#pragma once

#include <typeinfo>

#include "rules.hpp"
#include "closure.hpp"
#include "first.hpp"

namespace feiparser
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


    template<typename It>
    void parse_success(parse_state<It> &state, const std::type_info&)
    {

    }


    // Compute the list of tokens that this state is able to process
    // Anything else is a syntax error
    template<typename State>
    struct build_token_list
    {
        // TODO
        typedef typeset<token<0>, token<1>> type;
    };

    template<typename State, int Token, typename It>
    struct process_token
    {
        static void process(parse_state<It> & state)
        {
            
        }
    };

    template<typename Closure, typename Tokens, typename It>
    struct process_token_list;

    template<typename State, typename It>
    struct process_token_list<State, typeset<>, It>
    {
        static void process(parse_state<It> & state)
        {
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
        using Tokens = typename build_token_list<Closure>::type;
        
        process_token_list<State, Tokens, It>::process(state);
    }

    template<typename State, typename It>
    void reduce(parse_state<It> & state, const std::type_info & type)
    {
        // Jump to the next state from this state
    }

    template<typename Symbol, typename It>
    tree parse(const token_stream<It> &tokens)
    {
        parse_state<It> state(tokens);

        // Do the parsing
        using S0 = typeset<rule_position<Symbol, 0, 0>>;
        state.stack.push_back(parse_success);

        state.tokens.lex();
        parse<S0>(state);

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
