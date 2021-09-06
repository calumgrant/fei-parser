

namespace cellar
{
    /*
        Computes the action of a token `Token` in state `State`.
        The result is in the `type` member.

        The result is one of:
        - `shift<NewState>`
        - `accept`
        - `reduce<Rule>`
        - `error`
    */    

    template<int Token>
    struct shift {};

    template<typename Rule>
    struct reduce {};

    template<typename State, int Token>
    struct action2;

    template<int Token>
    struct action2<typeset<>, Token>
    {
        using type = typeset<>;
    };

    template<typename Rule, int Position, int Lookahead, int Token, typename OriginalRule>
    struct item_action2;

    template<int Id, typename Symbol, typename...Symbols, int Position, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<rule<Id, Symbol, Symbols...>, Position, Lookahead, Token, OriginalRule>
    {
        using type = typename item_action2<rule<Id, Symbols...>, Position-1, Lookahead, Token, OriginalRule>::type;
    };

    template<int Id, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<rule<Id>, 0, Lookahead, Token, OriginalRule>
    {
        using type = typeset<>;
    };

    template<int Id, typename...Def, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<rule<Id, token<Token, Def...>, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using type = typeset<shift<Token>>;
    };

    template<int Id, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<rule<Id, token<Token>, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using type = typeset<shift<Token>>;
    };

    template<int Id, typename Symbol, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<rule<Id, Symbol, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using type = typeset<>;
    };

    template<int Id, int Lookahead, typename OriginalRule>
    struct item_action2<rule<Id>, 0, Lookahead, Lookahead, OriginalRule>
    {
        using type = typeset<reduce<OriginalRule>>;
    };

    template<typename Item, int Token>
    struct item_action;

    template<typename Rule, int Position, int Lookahead, int Token>
    struct item_action<rule_position<Rule, Position, Lookahead>, Token>
    {
        using type = typename item_action2<Rule, Position, Lookahead, Token, Rule>::type;
    };

    template<typename Item, typename...Items, int Token>
    struct action2<typeset<Item, Items...>, Token>
    {
        using tail = typename action2<typeset<Items...>, Token>::type;

        using i = typename item_action<Item, Token>::type;
        using type = typename typeset_union<i, tail>::type;
    };

    template<typename State, int Token>
    struct action
    {
        using Closure = typename closure<State>::type;
        using type = typename action2<Closure, Token>::type;
    };


    template<typename Rule, int Position, int Token>
    struct shifts;

    template<int Id, typename Other, typename...Symbols, int Token>
    struct shifts<rule<Id, Other, Symbols...>, 0, Token>
    {
        static const bool value = false;
    };

    template<int Id, int Token, typename...Def, typename...Symbols>
    struct shifts<rule<Id, token<Token, Def...>, Symbols...>, 0, Token>
    {
        static const bool value = true;
    };

    template<int Id, int Token>
    struct shifts<rule<Id>, 0, Token>
    {
        static const bool value = false;
    };


    template<int Id, typename Symbol, typename...Symbols, int Position, int Token>
    struct shifts<rule<Id, Symbol, Symbols...>, Position, Token>
    {
        static const bool value = shifts<rule<Id, Symbols...>, Position-1, Token>::value;
    };


    template<typename State, int Token>
    struct shift_action2;

    template<int Token>
    struct shift_action2<typeset<>, Token>
    {
        using type = typeset<>;
    };

    template<typename Rule, int Position, int Lookahead, typename...Items, int Token>
    struct shift_action2<typeset<rule_position<Rule, Position, Lookahead>, Items...>, Token>
    {
        using T1 = rule_position<Rule, Position+1, Lookahead>;
        using T2 = typename shift_action2<typeset<Items...>, Token>::type;
        static const bool shiftsToken = shifts<Rule, Position, Token>::value;
        using type = typename type_if<shiftsToken, typename typeset_insert<T1, T2>::type, T2>::type;
    };

    template<typename State, int Token>
    struct shift_action
    {
        using Closure = typename closure<State>::type;
        using type = typename shift_action2<Closure, Token>::type;
    };

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

}