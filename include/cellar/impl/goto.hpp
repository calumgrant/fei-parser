namespace cellar
{
    namespace impl
    {
        template<typename Rule, int Position, typename Symbol>
        struct shifts_symbol;

        template<int Id, typename Other, typename...Symbols, typename Symbol>
        struct shifts_symbol<rule<Id, Other, Symbols...>, 0, Symbol>
        {
            static const bool value = false;
        };

        template<int Id, typename Symbol, typename...Symbols>
        struct shifts_symbol<rule<Id, Symbol, Symbols...>, 0, Symbol>
        {
            static const bool value = true;
        };

        template<int Id, typename Symbol>
        struct shifts_symbol<rule<Id>, 0, Symbol>
        {
            static const bool value = false;
        };


        template<int Id, typename Symbol, typename...Symbols, int Position, typename Symbol2>
        struct shifts_symbol<rule<Id, Symbol, Symbols...>, Position, Symbol2>
        {
            static const bool value = shifts_symbol<rule<Id, Symbols...>, Position-1, Symbol2>::value;
        };


        template<typename State, typename Symbol>
        struct goto_;

        template<typename Symbol>
        struct goto_<typeset<>, Symbol>
        {
            using type = typeset<>;
            using profile_tag = goto_tag;
            using profile_types = profile_types<Symbol>;
        };

        template<typename S, typename Rule, int Position, int Lookahead, typename...Items, typename Symbol>
        struct goto_<typeset<rule_position<S, Rule, Position, Lookahead>, Items...>, Symbol>
        {
            using T1 = rule_position<S, Rule, Position+1, Lookahead>;
            using T2 = typename goto_<typeset<Items...>, Symbol>::type;
            static const bool shiftsSymbol = shifts_symbol<Rule, Position, Symbol>::value;
            using type = typename type_if<shiftsSymbol, typename typeset_sorted_insert<T1, T2>::type, T2>::type;
            using profile_tag = goto_tag;
            using profile_types = profile_types<
                typeset<rule_position<S, Rule, Position, Lookahead>, Items...>,
                Symbol, T1, goto_<typeset<Items...>, Symbol>, /* shifts_symbol<Rule, Position, Symbol>, */
                typeset_sorted_insert<T1, T2> >;
        };
    }
    /*
        Computes the goto of symbol `Symbol` in state `State`.

       The result is the state to jump to, stored in the `type` member.
    */
    template<typename State, typename Symbol>
    struct goto_
    {
        using C = typename closure<State>::type;
        using T0 = typename impl::goto_<C, Symbol>::type;
        using type = typename typeset_sort<T0>::type;
        using profile_tag = goto_tag;
        using profile_types = profile_types<closure<State>, State, C, impl::goto_<C, Symbol>, typeset_sort<T0>>;

    };

    // TODO: Consolidate with is_symbol
    // and move away from here
    template<typename Symbol>
    struct is_symbol2
    {
        static const bool value = is_symbol2<typename Symbol::rules>::value;
    };

    template<int Token, typename...Def>
    struct is_symbol2<token<Token, Def...>>
    {
        static const bool value = false;
    };

    template<int Id, typename...Def>
    struct is_symbol2<rule<Id, Def...>>
    {
        static const bool value = true;
    };

    template<>
    struct is_symbol2<empty>
    {
        static const bool value = false;
    };

    template<typename...List>
    struct is_symbol2<symbol<List...>>
    {
        static const bool value = true;
    };



    template<typename Item>
    struct build_goto_item
    {
        using Symbol = typename impl::getnext<Item>::type;
        using type = typename type_if<is_symbol2<Symbol>::value, typeset<Symbol>, typeset<>>::type;

        using profile_tag = build_goto_list_tag;
        using profile_types = profile_types<Item, impl::getnext<Item>, typeset<Symbol>>;
    };

    template<typename State>
    struct build_goto_list2;

    template<>
    struct build_goto_list2<typeset<>>
    {
        using type = typeset<>;
        using profile_tag = build_goto_list_tag;
        using profile_types = profile_types<type>;
    };

    template<typename Item, typename...Items>
    struct build_goto_list2<typeset<Item, Items...>>
    {
        using T1 = typename build_goto_item<Item>::type;
        using T2 = typename build_goto_list2<typeset<Items...>>::type;
        using type = typename typeset_sorted_union<T1, T2>::type;

        using profile_tag = build_goto_list_tag;
        using profile_types = profile_types<typeset<Item, Items...>, build_goto_item<Item>, typeset_sorted_union<T1, T2>, type>;
    };

    template<typename State>
    struct build_goto_list
    {
        using C = typename closure<State>::type;
        using type = typename build_goto_list2<C>::type;

        using profile_tag = build_goto_list_tag;
        using profile_types = profile_types<State, closure<State>, C, build_goto_list2<C>, type>;
    };
}
