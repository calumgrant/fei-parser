#pragma once

#include <typeinfo>
#include <cassert>

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
#include "../output.hpp"
#endif

namespace cellar
{
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
        using type = typename make_list<token<Lookahead>>::type;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<rule_position<S, rule<Id>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, int Token, typename...Def, typename...Items, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, token<Token, Def...>, Items...>, 0, Lookahead>>
    {
        using type = typename make_list<token<Token>>::type;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<rule_position<S, rule<Id, token<Token, Def...>, Items...>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, typename Item, typename...Items, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, Item, Items...>, 0, Lookahead>>
    {
        using type = make_list<>::type;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<rule_position<S, rule<Id, Item, Items...>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, typename Item, typename...Items, int Position, int Lookahead>
    struct get_next_token<rule_position<S, rule<Id, Item, Items...>, Position, Lookahead>>
    {
        using type = typename get_next_token<rule_position<S, rule<Id, Items...>, Position-1, Lookahead>>::type;

        using profile_tag = get_next_token_tag;
        using profile_types = profile<
            rule_position<S, rule<Id, Item, Items...>, Position, Lookahead>, 
            get_next_token<rule_position<S, rule<Id, Items...>, Position-1, Lookahead>>,
            type>;
    };

    template<typename State, typename It>
    struct process_goto
    {
        // This function is called to implement the GOTO part of the table.
        // We need to compare the `type` of the symbol that was reduced
        // against the possible symbols `Symbol` in the GOTO table.
        // If the symbol matches (by comparing type info), then jump to that state `NextState`
        // and continue parsing 
        template<typename Symbol, typename Next>
        static void visit(parse_state<It> & state, const std::type_info & type)
        {
            if(size<Next>::value)
            {
                if(typeid(Symbol) == type)
                {
                    using NextState = typename goto_<State, Symbol>::type;
                    parse<NextState>(state);
                }
                else
                {
                    visitor<Next>::template visit<process_goto>(state, type);
                }
            }
            else
            {
                // End of the list
                // There is one possible symbol, so we don't need to test it.
                // If this fails, something has gone badly wrong in the parser construction algorithm.
                assert(typeid(Symbol) == type);
                using NextState = typename goto_<State, Symbol>::type;
                parse<NextState>(state);
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
        using C = typename closure<State>::type;
        using Gotos = typename build_goto_list<C>::type;
#if CELLAR_TRACE_PARSER
        std::cout << "Reduced to state " << C() << " with gotos = " << Gotos() << std::endl;
#endif
        visitor<Gotos>::template visit<process_goto<State, It>>(ps, rule);
        // process_goto<State, Gotos, It>::process(ps, rule);
    }

    // Compute the list of tokens that this state is able to process
    // Anything else is a syntax error
    template<typename Item, typename List>
    struct build_next_token_loop
    {
        using T1 = typename get_next_token<Item>::type;
        using type = typename merge<List, T1>::type;

        using profile_tag = build_next_token_list_tag;
        using profile_types = profile<
            Item,
            List,
            get_next_token<Item>,
            merge<List, T1>
            >;
    };

    template<typename Closure>
    struct build_next_token_list
    {
        using type = typename forall<Closure, make_list<>::type, build_next_token_loop>::type;

        using profile_tag = build_next_token_list_tag;
        using profile_types = profile<forall<Closure, make_list<>::type, build_next_token_loop>>;
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
            std::cout << "Shifting" << std::endl;
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
            std::cout << "Reducing " << Reduce() << std::endl;
#endif
            if(Rule::length==0)
                state.stack.push_back(&reduce_fn<State, It>);
                
            for(int i=1; i<Rule::length; ++i)
                state.stack.pop_back();
            state.parse_tree.reduce(Rule::id, Rule::length);
            auto fn = state.stack.back();
            fn(state, typeid(typename Reduce::symbol));
        }
    };

    template<typename State, typename It>
    struct process_token_list
    {
        template<typename Token, typename Next>
        static void visit(parse_state<It> & state)
        {
            if(state.tokens.token() == Token::id)
            {
                process_token<State, Token::id, It>::process(state);
            }
            else if(size<Next>::value != 0)
            {
                visitor<Next>::template visit<process_token_list>(state);
            }
            else
            {
                // This is the very last token
                // syntax error
                // Shift the error token onto the tree for debugging purposes
                auto node = state.parse_tree.shift(state.tokens.token(), state.tokens.begin().location, state.tokens.size());

                // Write the token into the parse tree
                auto p = node.c_str();
                for(auto ch : state.tokens)
                    *p++ = ch;

                // TODO: Report list of possible tokens
                state.parse_tree.SyntaxError(state.tokens.begin().location);

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
        std::cout << "Lookahead token = " << state.tokens.token() << " = ";
        for(auto ch : state.tokens)
            std::cout << ch;
        std::cout << std::endl;
        std::cout << "State = " << State() << std::endl;
        std::cout << "Closure = " << Closure() << std::endl;
        std::cout << "Possible tokens = " << Tokens() << std::endl;
#endif

        static_assert(size<Tokens>::value != 0);
        visitor<Tokens>::template visit<process_token_list<State, It>>(state);
    }

    class WholeProgram : public symbol<> {};

    template<typename S>
    struct initial_state
    {
        using type = rule_position<WholeProgram, rule<0, S, token<EndOfStream>>, 0, EndOfStream>;
    };

    template<typename Symbol, typename It>
    void parse(const token_stream<It> &tokens, tree&t)
    {
#if CELLAR_TRACE_PARSER
        std::cout << "\n\n=== Starting parse ===\n";
#endif

        parse_state<It> state(tokens);
        state.parse_tree = std::move(t);
        t.clear();

        using State0 = typename initial_state<Symbol>::type;

        state.tokens.lex();
        state.stack.push_back(parse_success);
        parse<State0>(state);

        t = std::move(state.parse_tree);
    }

    template<typename Lexer, typename Grammar, typename It>
    void parse(It start, It end, tree&t)
    {
        auto tokens = make_lexer<Lexer,linecounter<It>>().tokenize(start, end);
        parse<Grammar>(tokens, t);
    }

    template<typename Grammar, typename It = const char*>
    parser<It> make_parser(const lexer<linecounter<It>> & l)
    {
        return parser<It>(l, parse<Grammar>);
    }

    template<typename Grammar, typename Lexer = typename make_lexer_from_grammar<Grammar>::type, typename It = const char*>
    parser<It> make_parser()
    {
        return make_parser<Grammar>(make_lexer<Lexer, linecounter<It>>());
    }
}
