#pragma once

#include <typeinfo>

#include <iostream>  // Deleteme

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


    template<typename It>
    void parse_success(parse_state<It> &state, const std::type_info&)
    {

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

    template<typename State, typename It>
    void parse(parse_state<It> & state);

    template<typename State, int Token, typename It, 
        bool Shifts = is_shift<State, Token>::value,
        bool Reduces = is_reduce<State, Token>::value>
    struct process_token;

    template<typename State, int Token, typename It>
    struct process_token<State, Token, It, true, false>
    {
        static void process(parse_state<It> & state)
        {
            state.tokens.lex();
            using S2 = typename shift_action<State, Token>::type;
            return parse<S2>(state);
        }
    };

    template<typename State, int Token, typename It>
    struct process_token<State, Token, It, false, true>
    {
        static void process(parse_state<It> & state)
        {
            // TODO
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

    template<typename State, typename It>
    void reduceFn(parse_state<It> & state, const std::type_info & type)
    {
        // Jump to the next state from this state
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
