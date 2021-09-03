#pragma once

#include <typeinfo>

#include "rules.hpp"
#include "closure.hpp"

namespace feiparser
{

    /*
        Determines whether a grammar symbol can be empty.
        It needs to guard against recursion. 
    */
    template<typename Symbol, typename Visited = typeset<>, bool Recursive = typeset_contains<Symbol, Visited>::value>
    struct potentially_empty_symbol
    {
        static const bool value = potentially_empty_symbol<typename Symbol::rules, typename typeset_insert<Symbol, Visited>::type>::value;
    };

    template<typename Symbol, typename Visited>
    struct potentially_empty_symbol<Symbol, Visited, true>
    {
        // Recursive case
        static const bool value = false;
    };

    template<typename Visited>
    struct potentially_empty_symbol<symbol<>, Visited, false>
    {
        static const bool value = false;
    };

    template<typename Rule, typename...Rules, typename Visited>
    struct potentially_empty_symbol<symbol<Rule, Rules...>, Visited, false>
    {
        static const bool value = potentially_empty_symbol<Rule, Visited>::value || potentially_empty_symbol<symbol<Rules...>, Visited>::value;
    };

    template<int Id, typename...Definition, typename Visited>
    struct potentially_empty_symbol<token<Id, Definition...>, Visited, false>
    {
        static const bool value = false;
    };

    template<int Id, typename Visited>
    struct potentially_empty_symbol<rule<Id>, Visited, false>
    {
        static const bool value = true;
    };

    template<int Id, typename Rule, typename...Rules, typename Visited>
    struct potentially_empty_symbol<rule<Id, Rule, Rules...>, Visited, false>
    {
        static const bool value = potentially_empty_symbol<Rule, Visited>::value &&
            potentially_empty_symbol<rule<Id, Rules...>, Visited>::value;
    };

    /*
        Constructs a typeset of the "first" terminal/token symbol in a symbol.
        It needs to guard against recursion.
    */
    template<typename Symbol, typename Visited = typeset<>, bool Recursive = typeset_contains<Symbol, Visited>::value>
    struct first
    {
        // Base case is to look at the "rules" member of a user-defined type.
        using type = typename first<typename Symbol::rules, typename typeset_insert<Symbol, Visited>::type>::type;
    };

    template<typename Symbol, typename Visited>
    struct first<Symbol, Visited, true>
    {
        using type = typeset<>;
    };

    template<typename Visited>
    struct first<symbol<>, Visited, false>
    {
        using type = typeset<>;
    };

    template<typename Rule, typename... Rules, typename Visited>
    struct first<symbol<Rule, Rules...>, Visited, false>
    {
        using S1 = typename first<Rule, Visited>::type;
        using S2 = typename first<symbol<Rules...>, Visited>::type;
        using type = typename typeset_union<S1,S2>::type;
    };

    template<int Id, typename Visited>
    struct first<rule<Id>, Visited, false>
    {
        using type = typeset<>;        
    };

    template<int Id, typename...Def, typename Visited>
    struct first<token<Id, Def...>, Visited, false>
    {
        using type = typeset<token<Id>>;
    };

    template<int Id, typename Symbol, typename...Symbols, typename Visited>
    struct first<rule<Id, Symbol, Symbols...>, Visited, false>
    {
        using S1 = typename first<Symbol, Visited>::type;
        using S2 = typename first<rule<Id, Symbols...>, Visited>::type;
        using U = typename typeset_union<S1,S2>::type;
        using type = typename type_if<potentially_empty_symbol<Symbol>::value, U, S1>::type;
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
