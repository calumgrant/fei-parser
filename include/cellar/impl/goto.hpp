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


        template<typename Symbol>
        struct goto_loop
        {
            template<typename Item, typename NewState> struct loop;

            template<typename S, typename Rule, int Position, int Lookahead, typename T2>
            struct loop<rule_position<S, Rule, Position, Lookahead>, T2>
            {
                using T1 = rule_position<S, Rule, Position+1, Lookahead>;
                static const bool shiftsSymbol = shifts_symbol<Rule, Position, Symbol>::value;
                // TODO: Avoid type_if if possible
                using type = typename type_if<shiftsSymbol, typename insert<T1, T2>::type, T2>::type;
                using profile_tag = goto_tag;
                using profile_types = profile<
                    rule_position<S, Rule, Position, Lookahead>,
                    Symbol, T1, /* shifts_symbol<Rule, Position, Symbol>, */
                    insert<T1, T2> >;
            };

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
        using T0 = typename forall<C, empty_tree, impl::goto_loop<Symbol>::template loop>::type;
        using type = typename make_balanced_tree<T0>::type;

        using profile_tag = goto_tag;
        using profile_types = profile<closure<State>, State, C, forall<C, empty_tree, impl::goto_loop<Symbol>::template loop>, make_balanced_tree<T0>>;
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
        using type = typename type_if<is_symbol2<Symbol>::value, typename make_list<Symbol>::type, make_list<>::type>::type;

        using profile_tag = build_goto_list_tag;
        using profile_types = profile<Item, impl::getnext<Item>>;
    };


    template<typename Item, typename T2>
    struct build_goto_list2
    {
        using T1 = typename build_goto_item<Item>::type;
        using type = typename merge<T1, T2>::type;

        using profile_tag = build_goto_list_tag;
        using profile_types = profile<build_goto_item<Item>, type>;
    };


    template<typename State>
    struct build_goto_list
    {
        using C = typename closure<State>::type;

        using type = typename forall<C, make_list<>::type, build_goto_list2>::type;

        using profile_tag = build_goto_list_tag;
        using profile_types = profile<State, closure<State>, C, type>;
    };
}
