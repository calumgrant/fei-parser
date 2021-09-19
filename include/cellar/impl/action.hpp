

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
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<int Lookahead, typename Symbol, typename Rule>
    struct reduce
    {
        using symbol = Symbol;
        using rule = Rule;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    struct accept {};

    struct syntax_error {};

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
        using shift_actions = empty_tree;
        using reduce_actions = empty_tree;
    };

    template<typename S, int Id, typename...Def, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id, token<Token, Def...>, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using shift_actions = shift<Token, OriginalRule>;
        using reduce_actions = empty_tree;
    };

    template<typename S, int Id, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id, token<Token>, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using shift_actions = shift<Token, OriginalRule>;
        using reduce_actions = empty_tree;
    };

    template<typename S, int Id, typename Symbol, typename...Symbols, int Lookahead, int Token, typename OriginalRule>
    struct item_action2<S, rule<Id, Symbol, Symbols...>, 0, Lookahead, Token, OriginalRule>
    {
        using shift_actions = empty_tree;
        using reduce_actions = empty_tree;
    };

    template<typename S, int Id, int Lookahead, typename OriginalRule>
    struct item_action2<S, rule<Id>, 0, Lookahead, Lookahead, OriginalRule>
    {
        using shift_actions = empty_tree;
        using reduce_actions = reduce<Lookahead, S, OriginalRule>;
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


    template<int Token>
    struct action2_loop
    {
        template<typename Item, typename List>
        struct shift_action_loop
        {
            using type = typename tree_union<typename item_action<Item, Token>::shift_actions, List>::type;
        };

        template<typename Item, typename List>
        struct reduce_action_loop
        {
            using type = typename tree_union<typename item_action<Item, Token>::reduce_actions, List>::type;
        };
    };

    template<typename State, int Token>
    struct action
    {
        using Closure = typename closure<State>::type;
        using shift_actions = typename forall<State, empty_tree, action2_loop<Token>::template shift_action_loop>::type;
        using reduce_actions = typename forall<State, empty_tree, action2_loop<Token>::template reduce_action_loop>::type;
        using actions = typename tree_union<shift_actions, reduce_actions>::type;

        static const bool shifts = !type_equals<empty_tree, shift_actions>::value;
        static const bool reduces = !type_equals<empty_tree, reduce_actions>::value;
    };

    // Redundant??
    template<typename Rule, int Position, int Token>
    struct shifts;

    template<int Id, typename Other, typename...Symbols, int Token>
    struct shifts<rule<Id, Other, Symbols...>, 0, Token>
    {
        static const bool value = false;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<int Id, int Token, typename...Def, typename...Symbols>
    struct shifts<rule<Id, token<Token, Def...>, Symbols...>, 0, Token>
    {
        static const bool value = true;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<int Id, int Token>
    struct shifts<rule<Id>, 0, Token>
    {
        static const bool value = false;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };


    template<int Id, typename Symbol, typename...Symbols, int Position, int Token>
    struct shifts<rule<Id, Symbol, Symbols...>, Position, Token>
    {
        static const bool value = shifts<rule<Id, Symbols...>, Position-1, Token>::value;
        using profile_tag = no_tag;
        using profile_types = profile<shifts<rule<Id, Symbols...>, Position-1, Token>>;
    };

 
    template<int Token>
    struct shift_action_loop
    {
        template<typename Item, typename Actions>
        struct shift_action2;

        template<typename S, typename Rule, int Position, int Lookahead, typename T2>
        struct shift_action2<rule_position<S, Rule, Position, Lookahead>, T2>
        {
            using T1 = rule_position<S, Rule, Position+1, Lookahead>;
            static const bool shiftsToken = shifts<Rule, Position, Token>::value;
            using type = typename type_if<shiftsToken, typename tree_insert<T1, T2>::type, T2>::type;

            using profile_tag = shift_action_tag;
            using profile_types = profile<T1, shifts<Rule, Position, Token>, tree_insert<T1, T2>>;
        };
    };

    /*
        Computes the new state following a shift
    */
    template<typename State, int Token>
    struct shift_action
    {
        using Closure = typename closure<State>::type;

        using T0 = typename forall<Closure, empty_tree, shift_action_loop<Token>::template shift_action2>::type;
        using type = typename make_balanced_tree<T0>::type;

        using profile_tag = shift_action_tag;
        using profile_types = profile<State, Closure, closure<State>, forall<Closure, empty_tree, shift_action_loop<Token>::template shift_action2>, make_balanced_tree<T0>>;
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
