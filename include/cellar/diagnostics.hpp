/*
    Contains various utilities to help diagnose 
*/

#pragma once
#include "cellar.hpp"

namespace cellar
{
    template<typename State, typename Gathered = make_list<>::type, bool Exists = contains<State, Gathered>::value>
    struct gather_states;

    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, true>
    {
        using type = Gathered;
        using profile_tag = no_tag;
        using profile_types = profile<State, Gathered, contains<State, Gathered>>;
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

    template<typename Closure>
    struct gather_goto_states2
    {
        template<typename Goto, typename Gathered>
        struct visit
        {
            using NextState = typename goto_<Closure, Goto>::type;
            using type = typename gather_states<NextState, Gathered>::type;
        };

    };

    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, false>
    {
        using G0 = typename insert<State, Gathered>::type;

        using Closure = typename closure<State>::type;
        using Tokens = typename build_next_token_list<Closure>::type;
        using Gotos = typename build_goto_list<Closure>::type;

        using G1 = typename gather_shift_states<Closure, G0, Tokens>::type;
        using type = typename forall<Gotos, G1, gather_goto_states2<Closure>::template visit>::type;

        using profile_tag = no_tag;
        using profile_types = profile<
            contains<State, Gathered>,
            insert<State, Gathered>,
            closure<State>, 
            build_next_token_list<Closure>,
            build_goto_list<Closure>,
            gather_shift_states<Closure, G0, Tokens>
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
        static const int number_of_states = size<states>::value;

        using profile_tag = no_tag;
        using profile_types = profile<states, gather_states<S0>>;

        static void output_stats()
        {
            profile_template<parser_diagnostics>();
            output_profiler();
        }
    };

    template<typename G>
    struct output<parser_diagnostics<G>>
    {
        static void write(std::ostream & os)
        {
            parser_diagnostics<G>::output_stats();
        }
    };


    template<typename Symbol, typename Visited = make_list<>::type, bool Recursive = contains<Symbol, Visited>::value>
    struct find_all_symbols
    {
        using type = Visited;
    };

    namespace impl
    {
        template<typename Symbol, typename Symbols>
        struct find_all_symbols2;

        template<typename Symbols>
        struct find_all_symbols2<symbol<>, Symbols>
        {
            using type = Symbols;
        };

        template<typename H, typename... T, typename Symbols>
        struct find_all_symbols2<symbol<H, T...>, Symbols>
        {
            using type = typename find_all_symbols<H, typename find_all_symbols2<symbol<T...>, Symbols>::type>::type;
        };


        template<int Id, typename...Def, typename Symbols>
        struct find_all_symbols2<token<Id, Def...>, Symbols>
        {
            using type = typename insert<token<Id>, Symbols>::type;
        };

        template<int Id, typename Symbols>
        struct find_all_symbols2<rule<Id>, Symbols>
        {
            using type = Symbols;
        };

        template<int Id, typename H, typename...T, typename Symbols>
        struct find_all_symbols2<rule<Id, H, T...>, Symbols>
        {
            using type = typename find_all_symbols2<rule<Id, T...>, typename find_all_symbols<H, Symbols>::type>::type;
        };
    }

    template<typename Symbol, typename Visited>
    struct find_all_symbols<Symbol, Visited, false>
    {
        using type = typename impl::find_all_symbols2<typename Symbol::rules, typename insert<Symbol, Visited>::type>::type;
    };

    template<int Id, typename...Symbols, typename Visited>
    struct find_all_symbols<rule<Id, Symbols...>, Visited, false>
    {
        using type = typename impl::find_all_symbols2<rule<Id, Symbols...>, Visited>::type;
    };

    template<typename Symbol>
    struct empty_symbols_in_grammar
    {
        using Symbols = typename find_all_symbols<Symbol>::type;
        using type = typename filter<Symbols, potentially_empty_symbol>::type;
    };

    template<typename Symbol>
    struct empty_symbols_in_grammar2
    {
        using Symbols = typename find_all_symbols<Symbol>::type;
        using type = typename filter<Symbols, cellar_deleteme::potentially_empty_symbol>::type;
    };

}
