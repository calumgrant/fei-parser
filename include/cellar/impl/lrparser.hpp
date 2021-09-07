#pragma once

#include <typeinfo>

#include <iostream>

#include "rules.hpp"
#include "closure.hpp"
#include "action.hpp"
#include "goto.hpp"
#include "conflicts.hpp"

namespace cellar
{
    template<typename Symbol>
    struct tokens
    {

    };

    // Should really be hidden/moved from here
    template<typename It>
    struct parse_state
    {
        typedef void(*NextFn)(parse_state&, const std::type_info&);
        std::vector<NextFn> stack;
        tree parse_tree;
        token_stream<It> tokens;

        parse_state(const token_stream<It> & t) : tokens(t) {}
    };

    template<typename State, typename It>
    void parse(parse_state<It> & state);

    template<typename It>
    void parse_success(parse_state<It> &state, const std::type_info&)
    {
        state.parse_tree.success = true;
    }

    template<typename Item>
    struct get_next_token;

    template<int Id, int Lookahead>
    struct get_next_token<rule_position<rule<Id>, 0, Lookahead>>
    {
        using type = typeset<token<Lookahead>>;
    };

    template<int Id, int Token, typename...Def, typename...Items, int Lookahead>
    struct get_next_token<rule_position<rule<Id, token<Token, Def...>, Items...>, 0, Lookahead>>
    {
        using type = typeset<token<Token>>;
    };

    template<int Id, typename Item, typename...Items, int Lookahead>
    struct get_next_token<rule_position<rule<Id, Item, Items...>, 0, Lookahead>>
    {
        using type = typeset<>;
    };

    template<int Id, typename Item, typename...Items, int Position, int Lookahead>
    struct get_next_token<rule_position<rule<Id, Item, Items...>, Position, Lookahead>>
    {
        using type = typename get_next_token<rule_position<rule<Id, Items...>, Position-1, Lookahead>>::type;
    };

    template<typename State, typename Symbols, typename Iterator>
    struct process_goto;

    template<typename State, typename It>
    struct process_goto<State, typeset<>, It>
    {
        static void process(parse_state<It> & state, const std::type_info & type)
        {
            assert(!"Something has gone terribly wrong - unhandled symbol being reduced");
        }
    };

    template<typename State, typename Symbol, typename... Symbols, typename It>
    struct process_goto<State, typeset<Symbol, Symbols...>, It>
    {
        static void process(parse_state<It> & state, const std::type_info & type)
        {
            if(typeid(Symbol) == type)
            {
                // Shift this token
                using NextState = typename goto_<State, Symbol>::type;
                parse<NextState>(state);
            }
            else
            {
                process_goto<State, typeset<Symbols...>, It>::process(state, type);
            }
        }
    };

    /*
        This function has been called when a rule has been reduced.
        The reduced rule is stored in the "rule" parameter.
     */
    template<typename State, typename It>
    void reduce_fn(parse_state<It> & ps, const std::type_info &rule)
    {
        // Find all the gotos for a given state.
        using Gotos = typename build_goto_list<State>::type;
        std::cout << Gotos() << State() << std::endl;
        process_goto<State, Gotos, It>::process(ps, rule);
    }

    // Compute the list of tokens that this state is able to process
    // Anything else is a syntax error
    template<typename State>
    struct build_next_token_list;

    template<>
    struct build_next_token_list<typeset<>>
    {
        using type = typeset<>;
    };

    template<typename Item, typename... Items>
    struct build_next_token_list<typeset<Item, Items...>>
    {
        using T0 = typename build_next_token_list<typeset<Items...>>::type;
        using T1 = typename get_next_token<Item>::type;
        using type = typename typeset_union<T0, T1>::type;
    };

    template<typename State, int Token, typename It, 
        bool Shifts = resolve_conflicts<State, Token>::is_shift>
    struct process_token;

    template<typename State, int Token, typename It>
    struct process_token<State, Token, It, true>
    {
        static void process(parse_state<It> & state)
        {
            state.tokens.lex();
            using S2 = typename shift_action<State, Token>::type;
            state.stack.push_back(&reduce_fn<State, It>);
            
            // TODO: Compute the "goto" and push it onto the stack
            return parse<S2>(state);
        }
    };

    /*
        The reduce case
     */
    template<typename State, int Token, typename It>
    struct process_token<State, Token, It, false>
    {
        static void process(parse_state<It> & state)
        {
            using Reduce = typename resolve_conflicts<State, Token>::type;
            
            using Rule = typename Reduce::rule;
            
            std::cout << "Action = " << typename resolve_conflicts<State, Token>::type() << std::endl;
            for(int i=0; i<Rule::length; ++i)
                state.stack.pop_back();
            auto fn = state.stack.back();
            // state.stack.pop_back();
            fn(state, typeid(Rule));
        }
    };

    template<typename Closure, typename Tokens, typename It>
    struct process_token_list;

    template<typename State, typename It>
    struct process_token_list<State, typeset<>, It>
    {
        static void process(parse_state<It> & state)
        {
            // TODO: Report list of possible tokens
            state.parse_tree.SyntaxError(state.tokens.begin().location);
        }
    };

    template<typename State, int Id, typename...Tokens, typename It>
    struct process_token_list<State, typeset<token<Id>, Tokens...>, It>
    {
        static void process(parse_state<It> & state)
        {
            if(state.tokens.token() == Id)
            {
                process_token<State, Id, It>::process(state);
            }
            else
            {
                process_token_list<State, typeset<Tokens...>, It>::process(state);
            }
        }
    };

    template<typename State, typename It>
    void parse(parse_state<It> & state)
    {
        // Process one token from the input stream.
        using Closure = typename closure<State>::type;
        using Tokens = typename build_next_token_list<Closure>::type;

        std::cout << "Parsing token in state " << Closure() << std::endl;
        std::cout << "Tokens = " << Tokens() << std::endl;

        process_token_list<State, Tokens, It>::process(state);
    }

    template<typename S>
    struct initial_state
    {
        using type = typeset<rule_position<rule<0, S, token<EndOfStream>>, 0, EndOfStream>>;
    };

    template<typename Symbol, typename It>
    tree parse(const token_stream<It> &tokens)
    {
        parse_state<It> state(tokens);

        // Do the parsing
        using State0 = typename initial_state<Symbol>::type;
        state.stack.push_back(parse_success);

        state.tokens.lex();
        parse<State0>(state);

        return std::move(state.parse_tree);
    }

    template<typename Lexer, typename Grammar, typename It>
    tree parse(It start, It end)
    {
        return parse<Grammar>(make_lexer<Lexer,linecounter<It>>().tokenize(start, end));
    }

    template<typename Lexer, typename Grammar, typename It = const char*>
    parser<It> make_parser()
    {
        return parser<It>(parse<Lexer, Grammar>);
    }

}
