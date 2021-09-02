#pragma once

#include <typeinfo>

namespace feiparser
{
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


    // Should really be hidden/moved from here
    template<typename It>
    struct parse_state
    {
        typedef void(*NextFn)(const std::type_info&, parse_state&);
        std::vector<NextFn> stack;
        tree parse_tree;
        token_stream<It> tokens;

        parse_state(const token_stream<It> & t) : tokens(t) {}
    };

    template<typename Rule, typename It>
    void parsefn(parse_state<It> &)
    {

    }

    template<typename Rule, int Position, int Lookahead>
    struct rule_position {};

    template<typename State>
    struct shift
    {

    };

    template<typename State, int Ch>
    struct lalr_transition
    {
        /*
            This is the core of the algorithm.
            The action is either: shift, reduce, accept, or error.

        */

    };

    template<typename It>
    void parse_success(const std::type_info&, parse_state<It> &state)
    {

    }

    template<typename Symbol, typename It>
    tree parse(const token_stream<It> &tokens)
    {
        parse_state<It> state(tokens);

        // Do the parsing
        using S0 = typeset<rule_position<Symbol, 0, 0>>;
        state.stack.push_back(parse_success);

        // Run the main loop
        while(!state.stack.empty())
        {
            auto p = state.stack.back();
            state.stack.pop_back();
//            p();
        }


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