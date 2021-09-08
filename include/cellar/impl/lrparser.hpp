#pragma once

#include <typeinfo>


#include "rules.hpp"
#include "closure.hpp"
#include "action.hpp"
#include "goto.hpp"
#include "conflicts.hpp"
#include "tokens.hpp"

#ifndef CELLAR_TRACE_PARSER
#define CELLAR_TRACE_PARSER 0
#endif 

#if CELLAR_TRACE_PARSER
#include <iostream>
#endif

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

    template<typename S, int Id, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id>, 0, Lookahead>>
    {
        using type = typeset<token<Lookahead>>;
    };

    template<typename S, int Id, int Token, typename...Def, typename...Items, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, token<Token, Def...>, Items...>, 0, Lookahead>>
    {
        using type = typeset<token<Token>>;
    };

    template<typename S, int Id, typename Item, typename...Items, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, Item, Items...>, 0, Lookahead>>
    {
        using type = typeset<>;
    };

    template<typename S, int Id, typename Item, typename...Items, int Position, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, Item, Items...>, Position, Lookahead>>
    {
        using type = typename get_next_token<rule_position<S, rule<Id, Items...>, Position-1, Lookahead>>::type;
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

    template<typename State, typename Symbol, typename It>
    struct process_goto<State, typeset<Symbol>, It>
    {
        // Optimization when there is only one shift type
        // - there is no need to check the type as this should always be true.
        static void process(parse_state<It> & state, const std::type_info & type)
        {
            assert(typeid(Symbol) == type);
            using NextState = typename goto_<State, Symbol>::type;
            parse<NextState>(state);
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
        using C = typename closure<State>::type;
        using Gotos = typename build_goto_list<C>::type;
#if CELLAR_TRACE_PARSER
        std::cout << "Reduced to state " << C() << " with gotos = " << Gotos() << std::endl;
#endif
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
#if CELLAR_TRACE_PARSER
            std::cout << "Shifting token " << Token << std::endl;
#endif
            auto node = state.parse_tree.shift(Token, state.tokens.begin().location, state.tokens.size());

            // Write the token into the parse tree
            auto p = node.c_str();
            for(auto ch : state.tokens)
                *p++ = ch;

            state.tokens.lex();
            using S2 = typename shift_action<State, Token>::type;
            state.stack.push_back(&reduce_fn<State, It>);
            return parse<S2>(state);
        }
    };

    template<typename State, typename It>
    struct process_token<State, EndOfStream, It, true>
    {
        static void process(parse_state<It> & state)
        {
            // Parse success
            state.parse_tree.success = true;
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
            
#if CELLAR_TRACE_PARSER
            std::cout << "Action = " << Reduce() << std::endl;
#endif
            for(int i=1; i<Rule::length; ++i)
                state.stack.pop_back();
            state.parse_tree.reduce(Rule::id, Rule::length);
            auto fn = state.stack.back();
            fn(state, typeid(typename Reduce::symbol));
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

#if CELLAR_TRACE_PARSER
        std::cout << "Parsing token in state " << Closure() << std::endl;
        std::cout << "Possible tokens = " << Tokens() << std::endl;
#endif

        process_token_list<State, Tokens, It>::process(state);
    }

    struct WholeProgram {};

    template<typename S>
    struct initial_state
    {
        using type = typeset<rule_position<WholeProgram, rule<0, S, token<EndOfStream>>, 0, EndOfStream>>;
    };

    template<typename Symbol, typename It>
    void parse(const token_stream<It> &tokens, tree&t)
    {
        parse_state<It> state(tokens);
        state.parse_tree = std::move(t);
        t.clear();

        using State0 = typename initial_state<Symbol>::type;

        state.tokens.lex();
        parse<State0>(state);

        t = std::move(state.parse_tree);
    }

    template<typename Lexer, typename Grammar, typename It>
    void parse(It start, It end, tree&t)
    {
        auto tokens = make_lexer<Lexer,linecounter<It>>().tokenize(start, end);
        parse<Grammar>(tokens, t);
    }

    template<typename Grammar, typename Lexer = typename make_lexer_from_grammar<Grammar>::type, typename It = const char*>
    parser<It> make_parser()
    {
        return parser<It>(parse<Lexer, Grammar>);
    }
}
