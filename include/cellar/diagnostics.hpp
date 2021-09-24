/*
    Contains various utilities to help diagnose 
*/

#pragma once
#include "cellar.hpp"

namespace cellar
{
    template<typename State, typename Gathered = empty_tree, bool Exists = tree_contains<State, Gathered>::value>
    struct gather_states;

    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, true>
    {
        using type = Gathered;
        using profile_tag = no_tag;
        using profile_types = profile<State, Gathered, tree_contains<State, Gathered>>;
    };

    template<typename Closure, typename Gathered, typename Tokens>
    struct gather_shift_states;

    template<typename State, typename Gathered, typename Action>
    struct next_action_state;

    template<typename State, typename Gathered, int Token, typename Rule>
    struct next_action_state<State, Gathered, shift<Token, Rule>>
    {
        using NextState = typename shift_action<State, Token>::type;
        using type = typename gather_states<NextState, Gathered>::type;

        using profile_tag = next_action_state_tag;
        using profile_types = profile<
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
        using profile_types = profile<State, Gathered, reduce<Token, Symbol, Rule>>;
    };

    template<typename Closure>
    struct gather_shift_states2
    {
        template<typename Item, typename Gathered>
        struct loop
        {
            using Action = typename resolve_conflicts<Closure, Item::id>::type;
            using type = typename next_action_state<Closure, Gathered, Action>::type;
            using profile_tag = gather_shift_states_tag;
            using profile_types = profile<resolve_conflicts<Closure, Item::id>, next_action_state<Closure, Gathered, Action>>;
        };
    };

    template<typename Closure, typename Gathered, typename Tokens>
    struct gather_shift_states
    {
        using type = typename forall<Tokens, Gathered, gather_shift_states2<Closure>::template loop>::type;

        using profile_tag = gather_shift_states_tag;
        using profile_types = profile<forall<Tokens, Gathered, gather_shift_states2<Closure>::template loop>>;
    };

    template<typename Closure, typename Gathered, typename Gotos>
    struct gather_goto_states;

    template<typename Closure, typename Gathered>
    struct gather_goto_states<Closure, Gathered, typeset<>>
    {
        using type = Gathered;
        using profile_tag = no_tag;
        using profile_types = profile<Closure, Gathered>;
    };

    template<typename Closure, typename Gathered, typename Goto, typename...Gotos>
    struct gather_goto_states<Closure, Gathered, typeset<Goto, Gotos...>>
    {
        using G0 = typename gather_goto_states<Closure, Gathered, typeset<Gotos...>>::type;
        using NextState = typename goto_<Closure, Goto>::type;
        using type = typename gather_states<NextState, G0>::type;
        using profile_tag = no_tag;
        using profile_types = profile<
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
        using G0 = typename tree_insert<State, Gathered>::type;

        using Closure = typename closure<State>::type;
        using Tokens = typename build_next_token_list<Closure>::type;
        using Gotos = typename build_goto_list<Closure>::type;

        using G1 = typename gather_shift_states<Closure, G0, Tokens>::type;
        using type = typename gather_goto_states<Closure, G1, Gotos>::type;

        using profile_tag = no_tag;
        using profile_types = profile<
            tree_contains<State, Gathered>,
            tree_insert<State, Gathered>,
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
        insert_tag,
        merge_tag,
        contains_tag,
        size_tag,
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
        goto_tag,
        tree_tag,
        ints_tag
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
        "insert_tag",
        "merge_tag",
        "contains_tag",
        "size_tag",
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
        "goto_tag",
        "tree_tag",
        "ints_tag"
    };

    inline void output_profiler()
    {
        output_profiler_stats(profile_tags(), TagNames);
    }

    inline void reset_stats(typeset<>)
    {
    }

    template<typename Tag, typename...Tags>
    void reset_stats(typeset<Tag, Tags...>)
    {
        static_count<Tag>() = 0;
        reset_stats(typeset<Tags...>());
    }

    inline void reset_profiler_stats()
    {
        reset_stats(profile_tags());
    }

    template<typename Grammar>
    struct parser_diagnostics
    {
        using S0 = typename initial_state<Grammar>::type;
        using states = typename gather_states<S0>::type;
        static const int number_of_states = tree_size<states>::value;

        using profile_tag = no_tag;
        using profile_types = profile<states, gather_states<S0>>;

        static void output_stats()
        {
            profile_template<parser_diagnostics>();
            output_profiler();
        }
    };
}
