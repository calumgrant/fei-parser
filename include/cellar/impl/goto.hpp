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
        };

        template<typename Rule, int Position, int Lookahead, typename...Items, typename Symbol>
        struct goto_<typeset<rule_position<Rule, Position, Lookahead>, Items...>, Symbol>
        {
            using T1 = rule_position<Rule, Position+1, Lookahead>;
            using T2 = typename goto_<typeset<Items...>, Symbol>::type;
            static const bool shiftsSymbol = shifts_symbol<Rule, Position, Symbol>::value;
            using type = typename type_if<shiftsSymbol, typename typeset_insert<T1, T2>::type, T2>::type;
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
        using type = typename impl::goto_<C, Symbol>::type;
    };
}