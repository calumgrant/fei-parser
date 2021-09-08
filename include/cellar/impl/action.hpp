

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

    template<int Token, typename Rule>
    struct shift
    {
        using rule = Rule;
    };

    template<int Lookahead, typename Symbol, typename Rule>
    struct reduce
    {
        using symbol = Symbol;
        using rule = Rule;
    };

    struct accept {};

    struct syntax_error {};

    template<typename State, int Token>
    struct action2;

    template<int Token>
    struct action2<typeset<>, Token>
    {
        using shift_actions = typeset<>;
        using reduce_actions = typeset<>;
    };

    template<typename S, typename Rule, int Position, int Lookahead, int Token, typename OriginalRule>
    struct item_action2;

    template<typename S, int Id, typename Symbol, typename...Symbols, int Position, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id, Symbol, Symbols...>, Position, Lookahead, Token, OriginalRule>
    {
        using T = item_action2<S, rule<Id, Symbols...>, Position-1, Lookahead, Token, OriginalRule>;
        using shift_actions = typename T::shift_actions;
        using reduce_actions = typename T::reduce_actions;
    };

    template<typename S, int Id, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id>, 0, Lookahead, Token, OriginalRule>
    {
        using shift_actions = typeset<>;
        using reduce_actions = typeset<>;
    };

    template<typename S, int Id, typename...Def, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id, token<Token, Def...>, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using shift_actions = typeset<shift<Token, OriginalRule>>;
        using reduce_actions = typeset<>;
    };

    template<typename S, int Id, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id, token<Token>, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using shift_actions = typeset<shift<Token, OriginalRule>>;
        using reduce_actions = typeset<>;
    };

    template<typename S, int Id, typename Symbol, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id, Symbol, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using shift_actions = typeset<>;
        using reduce_actions = typeset<>;
    };

    template<typename S, int Id, int Lookahead, typename OriginalRule>
    struct item_action2<S, rule<Id>, 0, Lookahead, Lookahead, OriginalRule>
    {
        using shift_actions = typeset<>;
        using reduce_actions = typeset<reduce<Lookahead, S, OriginalRule>>;
    };

    template<typename Item, int Token>
    struct item_action;

    template<typename S, typename Rule, int Position, int Lookahead, int Token>
    struct item_action<rule_position<S, Rule, Position, Lookahead>, Token>
    {
        using T = item_action2<S, Rule, Position, Lookahead, Token, Rule>;
        using shift_actions = typename T::shift_actions;
        using reduce_actions = typename T::reduce_actions;
    };

    template<typename Item, typename...Items, int Token>
    struct action2<typeset<Item, Items...>, Token>
    {
        using tail = action2<typeset<Items...>, Token>;

        using i = item_action<Item, Token>;

        using shift_actions = typename typeset_union<typename i::shift_actions, typename tail::shift_actions>::type;
        using reduce_actions = typename typeset_union<typename i::reduce_actions, typename tail::reduce_actions>::type;
    };

    template<typename State, int Token>
    struct action
    {
        using Closure = typename closure<State>::type;
        using T = action2<Closure, Token>;
        using shift_actions = typename T::shift_actions;
        using reduce_actions = typename T::reduce_actions;
        using actions = typename typeset_union<shift_actions, reduce_actions>::type;

        static const bool shifts = !is_empty(shift_actions());
        static const bool reduces = !is_empty(reduce_actions());
    };

    // Redundant??
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

    template<typename S, typename Rule, int Position, int Lookahead, typename...Items, int Token>
    struct shift_action2<typeset<rule_position<S, Rule, Position, Lookahead>, Items...>, Token>
    {
        using T1 = rule_position<S, Rule, Position+1, Lookahead>;
        using T2 = typename shift_action2<typeset<Items...>, Token>::type;
        static const bool shiftsToken = shifts<Rule, Position, Token>::value;
        using type = typename type_if<shiftsToken, typename typeset_insert<T1, T2>::type, T2>::type;
    };

    /*
        Computes the new state following a shift
    */
    template<typename State, int Token>
    struct shift_action
    {
        using Closure = typename closure<State>::type;
        using type = typename shift_action2<Closure, Token>::type;
    };

    template<typename State, int Token, typename Action = typename shift_action<State, Token>::type>
    struct is_shift
    {
        static const bool value = typeset_equals<Action, typeset<>>::value;
    };

    /*
        Computes the rule to reduce
    */
    template<typename State, int Token>
    struct reduce_action
    {
        using type = typename action<State, Token>::reduce_actions;
    };

    template<typename State, int Token, typename Action = typename reduce_action<State, Token>::type>
    struct is_reduce
    {
        static const bool value = typeset_equals<Action, typeset<>>::value;
    };

}
