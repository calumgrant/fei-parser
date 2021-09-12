/*
    Contains various utilities to help diagnose 
*/

#pragma once
#include "impl/lrparser.hpp"

namespace cellar
{
    template<typename State, typename Gathered = typeset<>, bool Exists = typeset_contains<State, Gathered>::value>
    struct gather_states;

    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, true>
    {
        using type = Gathered;
    };

    template<typename Closure, typename Gathered, typename Tokens>
    struct gather_shift_states;

    template<typename Closure, typename Gathered>
    struct gather_shift_states<Closure, Gathered, typeset<>>
    {
        using type = Gathered;
    };

    template<typename State, typename Gathered, typename Action>
    struct next_action_state;

    template<typename State, typename Gathered, int Token, typename Rule>
    struct next_action_state<State, Gathered, shift<Token, Rule>>
    {
        using NextState = typename shift_action<State, Token>::type;
        using type = typename gather_states<NextState, Gathered>::type;
    };

    template<typename State, typename Gathered, int Token, typename Rule, typename Symbol>
    struct next_action_state<State, Gathered, reduce<Token, Symbol, Rule>>
    {
        using type = Gathered;
    };

    template<typename Closure, typename Gathered, int Token, typename... Tokens>
    struct gather_shift_states<Closure, Gathered, typeset<token<Token>, Tokens...>>
    {
        using Action = typename resolve_conflicts<Closure, Token>::type;

        using G0 = typename gather_shift_states<Closure, Gathered, typeset<Tokens...>>::type;

        using type = typename next_action_state<Closure, G0, Action>::type;
    };

    template<typename Closure, typename Gathered, typename Gotos>
    struct gather_goto_states;

    template<typename Closure, typename Gathered>
    struct gather_goto_states<Closure, Gathered, typeset<>>
    {
        using type = Gathered;
    };

    template<typename Closure, typename Gathered, typename Goto, typename...Gotos>
    struct gather_goto_states<Closure, Gathered, typeset<Goto, Gotos...>>
    {
        using NextState = typename goto_<Closure, Goto>::type;
        using type = typename gather_states<NextState, Gathered>::type;
    };


    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, false>
    {
        using G0 = typename typeset_insert<State, Gathered>::type;

        using Closure = typename closure<State>::type;
        using Tokens = typename build_next_token_list<Closure>::type;
        using Gotos = typename build_goto_list<Closure>::type;

        using G1 = typename gather_shift_states<Closure, G0, Tokens>::type;
        using type = typename gather_goto_states<Closure, G1, Gotos>::type;
    };

    template<typename Grammar>
    struct parser_diagnostics
    {
        using S0 = typename initial_state<Grammar>::type;
        using states = typename gather_states<S0>::type;
        static const int number_of_states = typeset_size<states>::value;
    };
}
