#pragma once

namespace cellar
{

    template<int Id>
    struct mark_shift_reduce_conflict
    {
        static const bool marked = false; 
    };

    struct marked_conflict
    {
        static const bool marked = true;
    };

    template<typename Item1, typename Item2>
    struct report_reduce_reduce_conflict
    {
        // static_assert(false, "Reduce/reduce conflict detected");
    };

    template<typename Item1, typename Item2, bool Marked>
    struct report_shift_reduce_conflict
    {
        static_assert(!Marked, "Shift/reduce conflict detected  - use mark_conflict<> to whitelist");
    };

    // Specialise this predicate to avoid reporting shift-reduce conflicts
    template<int Rule1, int Rule2>
    struct mark_conflict
    {
        static_assert(Rule1!=Rule1, "Shift-reduce conflict detected");
    };

    template<int Id, typename... Symbols>
    constexpr int ruleid(rule<Id, Symbols...>)
    {
        return Id;
    }

    template<int Id, typename S, int Token, typename... Symbols>
    constexpr int ruleid(reduce<Token, S, rule<Id, Symbols...>>)
    {
        return Id;
    }

    template<typename T>
    constexpr bool ignore_conflicts(T) { return false; }

    template<typename Shift, typename Reduce>
    struct shift_reduce_conflict
    {
        static const bool b = type_equals(Shift(),Reduce());
        static_assert(b, "Shift/reduce conflict detected in grammar");
        using resolution = Shift;
    };

    template<typename Reduce1, typename Reduce2>
    struct reduce_reduce_conflict
    {
        using resolution = typename type_if<
            ruleid(Reduce1()) < ruleid(Reduce2()),
            Reduce1, Reduce2>::type;

        static_assert(ignore_conflicts(Reduce1()) || ignore_conflicts(Reduce2()), "Reduce/reduce conflict detected in grammar");
    };



    template<typename Action1, typename Action2>
    struct resolve_actions;

    template<typename Action>
    struct resolve_actions<syntax_error, Action>
    {
        using type = Action;
    };

    template<int Id1, typename S1, typename Rule1, int Id2, typename S2, typename Rule2>
    struct resolve_actions<reduce<Id1, S1, Rule1>, reduce<Id2, S2, Rule2>>
    {
        using type = typename reduce_reduce_conflict<reduce<Id1, S1, Rule1>, reduce<Id2, S2, Rule2>>::resolution;
    };

    template<int Id1, typename S1, typename Rule1, int Id2, typename Rule2>
    struct resolve_actions<reduce<Id1, S1, Rule1>, shift<Id2, Rule2>>
    {
        using type = typename shift_reduce_conflict<shift<Id2, Rule2>, reduce<Id1, S1, Rule1>>::resolution;
    };

    template<int Id1, typename Rule1, int Id2, typename Rule2>
    struct resolve_actions<shift<Id1, Rule1>, shift<Id2, Rule2>>
    {
        using type = shift<Id1, Rule1>;
    };

    template<typename Action, typename Actions>
    struct resolve_action_set;

    template<typename Action>
    struct resolve_action_set<Action, typeset<>>
    {
        using type = Action;
    };

    template<typename Action, typename A, typename...As>
    struct resolve_action_set<Action, typeset<A, As...>>
    {
        using T = typename resolve_action_set<Action, typeset<As...>>::type;
        using type = typename resolve_actions<T, A>::type;
    };

    template<typename Action>
    struct is_shift2
    {
        static const bool value = false;
    };

    template<int Token, typename Rule>
    struct is_shift2<shift<Token, Rule>>
    {
        static const bool value = true;
    };

    template<typename Action>
    struct is_reduce2
    {
        static const bool value = false;
    };

    template<int Token, typename S, typename Rule>
    struct is_reduce2<reduce<Token, S, Rule>>
    {
        static const bool value = true;
    };


    /*
        Where there are several possible actions,
        - report the error if needed
        - resolve to just one action
    */
    template<typename State, int Token>
    struct resolve_conflicts
    {
        using PossibleActions = action<State, Token>;
        using A1 = typename resolve_action_set<syntax_error, typename PossibleActions::reduce_actions>::type;

        // The resolved action to take
        using type = typename resolve_action_set<A1, typename PossibleActions::shift_actions>::type;

        static const bool is_shift = cellar::is_shift2<type>::value;
        static const bool is_reduce = cellar::is_reduce2<type>::value;
    };
}
