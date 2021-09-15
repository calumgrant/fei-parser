/*
    Contains various utilities to help diagnose 
*/

#pragma once
#include "cellar.hpp"

namespace cellar
{
    template<typename State, typename Gathered = typeset<>, bool Exists = typeset_contains<State, Gathered>::value>
    struct gather_states;

    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, true>
    {
        using type = Gathered;
        using profile_tag = no_tag;
        using profile_types = profile_types<State, Gathered, typeset_contains<State, Gathered>>;
    };

    template<typename Closure, typename Gathered, typename Tokens>
    struct gather_shift_states;

    template<typename Closure, typename Gathered>
    struct gather_shift_states<Closure, Gathered, typeset<>>
    {
        using type = Gathered;

        using profile_tag = gather_shift_states_tag;
        using profile_types = profile_types<Closure, Gathered, typeset<>>;
    };

    template<typename State, typename Gathered, typename Action>
    struct next_action_state;

    template<typename State, typename Gathered, int Token, typename Rule>
    struct next_action_state<State, Gathered, shift<Token, Rule>>
    {
        using NextState = typename shift_action<State, Token>::type;
        using type = typename gather_states<NextState, Gathered>::type;

        using profile_tag = next_action_state_tag;
        using profile_types = profile_types<
            State, Gathered, NextState, 
            shift_action<State, Token>,
            gather_states<NextState, Gathered>
            >;
    };

    template<typename State, typename Gathered, int Token, typename Rule, typename Symbol>
    struct next_action_state<State, Gathered, reduce<Token, Symbol, Rule>>
    {
        using type = Gathered;

        using profile_tag = next_action_state_tag;
        using profile_types = profile_types<State, Gathered, reduce<Token, Symbol, Rule>>;
    };

    template<typename Closure, typename Gathered, int Token, typename... Tokens>
    struct gather_shift_states<Closure, Gathered, typeset<token<Token>, Tokens...>>
    {
        using Action = typename resolve_conflicts<Closure, Token>::type;

        using G0 = typename gather_shift_states<Closure, Gathered, typeset<Tokens...>>::type;

        using type = typename next_action_state<Closure, G0, Action>::type;

        using profile_tag = gather_shift_states_tag;
        using profile_types = profile_types<Closure, Gathered, typeset<token<Token>, Tokens...>,
            resolve_conflicts<Closure, Token>,
            gather_shift_states<Closure, Gathered, typeset<Tokens...>>,
            next_action_state<Closure, G0, Action>
            >;

    };

    template<typename Closure, typename Gathered, typename Gotos>
    struct gather_goto_states;

    template<typename Closure, typename Gathered>
    struct gather_goto_states<Closure, Gathered, typeset<>>
    {
        using type = Gathered;
        using profile_tag = no_tag;
        using profile_types = profile_types<Closure, Gathered>;
    };

    template<typename Closure, typename Gathered, typename Goto, typename...Gotos>
    struct gather_goto_states<Closure, Gathered, typeset<Goto, Gotos...>>
    {
        using G0 = typename gather_goto_states<Closure, Gathered, typeset<Gotos...>>::type;
        using NextState = typename goto_<Closure, Goto>::type;
        using type = typename gather_states<NextState, G0>::type;
        using profile_tag = no_tag;
        using profile_types = profile_types<
            Closure,
            Gathered,
            typeset<Goto, Gotos...>,
            gather_goto_states<Closure, Gathered, typeset<Gotos...>>,
            goto_<Closure, Goto>,
            gather_states<NextState, G0>
            >;
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

        using profile_tag = no_tag;
        using profile_types = profile_types<
            typeset_contains<State, Gathered>,
            typeset_insert<State, Gathered>,
            closure<State>, 
            build_next_token_list<Closure>,
            build_goto_list<Closure>,
            gather_shift_states<Closure, G0, Tokens>,
            gather_goto_states<Closure, G1, Gotos>
            >;
    };

// Unused
    template<typename Tag>
    struct profile_tag {};

    template<typename Tag>
    std::ostream & operator<<(std::ostream & os, profile_tag<Tag>)
    {
        return os << static_count<Tag>();
    }

    inline void output_profiler_stats(typeset<>, const char **) {}

    template<typename T, typename... Ts>
    void output_profiler_stats(typeset<T, Ts...>, const char ** name)
    {
        std::cout << *name << " = " << static_count<T>() << std::endl;
        output_profiler_stats(typeset<Ts...>(), name+1);
    }

    using profile_tags = typeset<
        add_to_closure_tag,
        build_closure_tag,
        closure_tag,
        token_tag,
        rule_tag,
        rule_position_tag,
        no_tag,
        typeset_tag,
        typeset_insert_tag,
        typeset_union_tag,
        typeset_contains_tag,
        typeset_size_tag,
        typeset_sort_tag,
        typeset_sorted_insert_tag,
        typeset_sorted_union_tag,
        follow_tag,
        first_tag,
        potentially_empty_symbol_tag,
        getnext_tag,
        expand_symbol_tag,
        build_next_token_list_tag,
        get_next_token_tag,
        build_goto_list_tag,
        gather_shift_states_tag,
        resolve_conflicts_tag,
        next_action_state_tag,
        shift_action_tag,
        goto_tag
    >;

    static const char * TagNames[] = {
        "add_to_closure_tag",
        "build_closure_tag",
        "closure_tag",
        "token_tag",
        "rule_tag",
        "rule_position_tag",
        "no_tag",
        "typeset_tag",
        "typeset_insert_tag",
        "typeset_union_tag",
        "typeset_contains_tag",
        "typeset_size_tag",
        "typeset_sort_tag",
        "typeset_sorted_insert_tag",
        "typeset_sorted_union_tag",
        "follow_tag",
        "first_tag",
        "potentially_empty_symbol_tag",
        "getnext_tag",
        "expand_symbol_tag",
        "build_next_token_list_tag",
        "get_next_token_tag",
        "build_goto_list_tag",
        "gather_shift_states_tag",
        "resolve_conflicts_tag",
        "next_action_state_tag",
        "shift_action_tag",
        "goto_tag"
    };

    template<typename Grammar>
    struct parser_diagnostics
    {
        using S0 = typename initial_state<Grammar>::type;
        using states = typename gather_states<S0>::type;
//    using debug = typename gather_states<S0>::Gotos;
//    using debug2 = typename gather_goto_states<S0, typeset<>, debug>::NextState;
        static const int number_of_states = typeset_size<states>::value;

        using profile_tag = no_tag;
        using profile_types = profile_types<states, gather_states<S0>>;


        static void output_stats()
        {
            profile_template<parser_diagnostics>();
            output_profiler_stats(profile_tags(), TagNames);
        }
    };
}
