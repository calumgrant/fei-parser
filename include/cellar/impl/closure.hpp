#pragma once

/*
    This file has to do with building "closures".

    For example, for every item `A -> B . C ..., x`
    we need to "add" all of the rules in `C` as well.

    We need the "closure" to figure out which symbols can be shifted at each state.
*/

#include "typeset.hpp"
#include "potentially_empty_symbol.hpp"
#include "follow.hpp"
#include "sorted_insert.hpp"

namespace cellar
{
    template<typename L, typename...Ls, typename R, typename...Rs>
    struct compare<typeset<L, Ls...>, typeset<R, Rs...>>
    {
        using C1 = compare<L,R>;
        using C2 = compare<typeset<Ls...>, typeset<Rs...>>;

        static const bool equal = C1::equal && C2::equal;
        static const bool less = C1::less || (C1::equal && C2::less);
    };

    template<typename S1, int Id1, typename... R1, int P1, int L1, typename S2, int Id2, typename... R2, int P2, int L2>
    struct compare<rule_position<S1, rule<Id1, R1...>, P1, L1>, rule_position<S2, rule<Id2,R2...>, P2, L2>>
    {
        static const bool equal = false; // Would have been matched by previous case
        static const bool less = Id1<Id2 || (Id1==Id2 && (P1<P2 || (P1==P2 && (L1<L2 || (L1==L2 && compare<typeset<R1...>, typeset<R2...>>::less)))));
    };

    namespace impl
    {
        /*
            Loops through the items in the `Items`, adding them to the closure `type`.
        */
        template<typename Items, typename Closure>
        struct build_closure;

        /*
            Finds the next symbol in an Item (the symbol to the right of the `.`)
        */
        template<typename Item>
        struct getnext;

        /*
            Expands the symbol `Symbol`, with `Rules` being the rules in the symbol.

            `Follows` is the typeset of all symbols that follow the item being added, or
            possibly a single token `token<>`.
        */
        template<typename Symbol, typename Rules, typename Follows, typename Closure>
        struct expand_symbol;
    
        // ?? Delete
        template<typename Item>
        struct item;

        /*
            Adds a new item to a closure `Closure`, giving the result in `type`.
            `Rule` is a `rule<>` or possibly a `token<>`, that is turned into
            an item by setting its position to 0, and .

            - I think we can get rid of this one and fold it into
        */
        template<typename Symbol, typename Rule, typename Follows, typename Closure>
        struct add_rule;

        template<typename Item>
        struct skip_symbol;

        /*
            Adds and item `Item` to `Closure`, returning the new closure in `type`.
         */
        template<typename Item, typename Closure, bool NeedToExpand = !typeset_contains<Item, Closure>::value && item<Item>::reads_symbol>
        struct add_to_closure;


        /*
            Holds if `Symbol` is a symbol (and not a token)
         */
        template<typename Symbol>
        struct is_symbol;









    template<typename S, int Id, int Lookahead, typename Symbol, typename...Symbols>
    struct getnext<rule_position<S, rule<Id, Symbol, Symbols...>, 0, Lookahead>>
    {
        using type = Symbol;
        using type_or_lookahead = type;

        using profile_tag = getnext_tag;
        using profile_types = profile<rule_position<S, rule<Id, Symbol, Symbols...>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, int Position, int Lookahead, typename Symbol, typename...Symbols>
    struct getnext<rule_position<S, rule<Id, Symbol, Symbols...>, Position, Lookahead>>
    {
        using type = typename getnext<rule_position<S, rule<Id, Symbols...>, Position-1, Lookahead>>::type;
        using type_or_lookahead = type;

        using profile_tag = getnext_tag;
        using profile_types = profile<rule_position<S, rule<Id, Symbol, Symbols...>, Position, Lookahead>, type>;
    };

    template<typename S, int Id, int Position, int Lookahead>
    struct getnext<rule_position<S, rule<Id>, Position, Lookahead>>
    {
        using type = empty;

        using profile_tag = getnext_tag;
        using profile_types = profile<rule_position<S, rule<Id>, Position, Lookahead>, type>;
    };

    template<typename S, int Id, int Lookahead>
    struct getnext<rule_position<S, rule<Id>, 0, Lookahead>>
    {
        using type = empty;
        using type_or_lookahead = token<Lookahead>;

        using profile_tag = getnext_tag;
        using profile_types = profile<rule_position<S, rule<Id>, 0, Lookahead>, type>;
    };

    template<typename S, typename Follows, typename Closure>
    struct expand_symbol<S, symbol<>, Follows, Closure>
    {
        using type = Closure;

        using profile_tag = expand_symbol_tag;
        using profile_types = profile<S, symbol<>, Follows, Closure>;
    };

    template<typename S, typename Item, typename Follows, typename Closure>
    struct expand_symbol
    {
        using type = typename expand_symbol<S, typename Item::rules, Follows, Closure>::type;

        using profile_tag = expand_symbol_tag;
        using profile_types = profile<S, Item, Follows, Closure, 
            expand_symbol<S, typename Item::rules, Follows, Closure>, type>;
    };

    
    template<typename S, typename Item, typename...Items, typename Follows, typename Closure>
    struct expand_symbol<S, symbol<Item, Items...>, Follows, Closure>
    {
        using C1 = typename expand_symbol<S, Item, Follows, Closure>::type;
        using type = typename expand_symbol<S, symbol<Items...>, Follows, C1>::type;

        using profile_tag = expand_symbol_tag;
        using profile_types = profile<
            S, symbol<Item, Items...>, Follows, Closure,
            expand_symbol<S, Item, Follows, Closure>,
            expand_symbol<S, symbol<Items...>, Follows, C1>
         >;
    };

    template<typename S, int Id, typename...Rule, typename Follows, typename Closure>
    struct expand_symbol<S, token<Id, Rule...>, Follows, Closure>
    {
        using T = rule<Id, token<Id, Rule...>>;
        using type = typename expand_symbol<S, T, Follows, Closure>::type;

        using profile_tag = expand_symbol_tag;
        using profile_types = profile<
            S, token<Id, Rule...>, Follows, Closure,
            expand_symbol<S, T, Follows, Closure>
            >;
    };

    template<typename S, typename...Items1, typename...Items2, typename Follows, typename Closure>
    struct expand_symbol<S, symbol<symbol<Items1...>, Items2...>, Follows, Closure>
    {
        using T = typename expand_symbol<S, symbol<Items1...>, Follows, Closure>::type;
        using type = typename expand_symbol<S, symbol<Items2...>, Follows, T>::type;

        using profile_tag = expand_symbol_tag;
        using profile_types = profile<
            S, symbol<symbol<Items1...>, Items2...>, Follows, Closure,
            expand_symbol<S, symbol<Items1...>, Follows, Closure>,
            expand_symbol<S, symbol<Items2...>, Follows, T>
            >;
    };

    template<typename S, int Id, typename... Rule, typename... Follows, typename Closure, int Lookahead>
    struct expand_symbol<S, rule<Id, Rule...>, typeset<token<Lookahead>, Follows...>, Closure>
    {
        using Item = rule_position<S, rule<Id, Rule...>, 0, Lookahead>;
        using T = typename expand_symbol<S, rule<Id, Rule...>, typeset<Follows...>, Closure>::type;
        using type = typename add_to_closure<Item, T>::type;

        using profile_tag = expand_symbol_tag;
        using profile_types = profile<
            S, rule<Id, Rule...>, typeset<token<Lookahead>, Follows...>, Closure,
            expand_symbol<S, rule<Id, Rule...>, typeset<Follows...>, Closure>,
            add_to_closure<Item, T>
            >;
    };

    template<typename S, int Id, typename... Rule, typename Closure>
    struct expand_symbol<S, rule<Id, Rule...>, typeset<>, Closure>
    {
        // Empty follows set - base case
        using type = Closure;

        using profile_tag = expand_symbol_tag;
        using profile_types = profile<
            S, rule<Id, Rule...>, typeset<>, Closure>;
    };

    template<typename Item, typename Closure>
    struct add_to_closure<Item, Closure, false>
    {
        using type = typename typeset_sorted_insert<Item, Closure>::type;

        using profile_tag = add_to_closure_tag;
        using profile_types = profile<
            typeset_contains<Item, Closure>,
            typeset_sorted_insert<Item, Closure>,
            type
            >;
    };

    template<typename Symbol>
    struct is_symbol
    {
        static const bool value = is_symbol<typename Symbol::rules>::value;
    };

    template<int Id, typename...X>
    struct is_symbol<token<Id, X...>>
    {
        static const bool value = false;
    };

    template<int Id, typename...Ss>
    struct is_symbol<rule<Id, Ss...>>
    {
        static const bool value = true;
    };

    template<typename...X>
    struct is_symbol<symbol<X...>>
    {
        static const bool value = true;
    };

    template<>
    struct is_symbol<empty>
    {
        static const bool value = false;
    };


    template<typename S, typename Rule, int Position, int Lookahead>
    struct skip_symbol<rule_position<S, Rule, Position, Lookahead>>
    {
        using type = rule_position<S, Rule, Position+1, Lookahead>;
    };
    

    template<typename S, int Id, typename Symbol, typename... Symbols, int Position, int Lookahead>
    struct item<rule_position<S, rule<Id, Symbol, Symbols...>, Position, Lookahead>>
    {
        using rule_type = rule<Id, Symbol, Symbols...>;
        static const int lookahead = Lookahead;
        static const int position = Position;

        static const bool reads_symbol = item<rule_position<S, rule<Id, Symbols...>, Position-1, Lookahead>>::reads_symbol;
    };

    template<typename S, int Id, int Lookahead, typename Symbol, typename... Symbols>
    struct item<rule_position<S, rule<Id, Symbol, Symbols...>, 0, Lookahead>>
    {
        using rule_type = rule<Id, Symbol, Symbols...>;
        static const int lookahead = Lookahead;
        static const int position = 0;

        static const bool reads_symbol = is_symbol<Symbol>::value;
    };

    template<typename S, int Id, int Lookahead>
    struct item<rule_position<S, rule<Id>, 0, Lookahead>>
    {
        using rule_type = rule<Id>;
        static const int lookahead = Lookahead;
        static const int position = 0;

        static const bool reads_symbol = false;
    };


    /*
        Adds an item (i.e. a rule_position) to a closure, giving the new closure in the
        `type` member.

        If the item contains `. x` (the next symbol is a terminal), then the item
        does not need to be expanded, and is simply added to the closure.

        If the item contains `. X` (the next symbol is a non-terminal), 
        then we need to expand the symbol, using the lookahead of the following symbol.

        All newly added items also need to be expanded.
    */
    template<typename Item, typename Closure>
    struct add_to_closure<Item, Closure, true>
    {
        // Insert Item into Closure
        using C1 = typename typeset_sorted_insert<Item, Closure>::type;

        using Follows = typename follow<Item>::type;

        using NextSymbol= typename getnext<Item>::type;

        //using C2 = typename type_if<
        //    potentially_empty_symbol<NextSymbol>::value,
        //    typename add_to_closure<typename skip_symbol<Item>::type, C1>::type,
        //  C1>::type;

        using type = typename expand_symbol<NextSymbol, typename NextSymbol::rules, Follows, C1>::type;

        using profile_tag = add_to_closure_tag;
        using profile_types = profile<
            typeset_contains<Item, Closure>,
            typeset_sorted_insert<Item, Closure>, 
            follow<Item>,
            getnext<Item>,
            expand_symbol<NextSymbol, typename NextSymbol::rules, Follows, C1>,
            type>;
    };

    template<typename Closure>
    struct build_closure<typeset<>, Closure>
    {
        using type = Closure;
        using profile_tag = build_closure_tag;
        using profile_types = profile<typeset<>, Closure>;
    };

    template<typename Item, typename...Items, typename Closure>
    struct build_closure<typeset<Item, Items...>, Closure>
    {
        using C1 = typename build_closure<typeset<Items...>, Closure>::type;
        using type = typename add_to_closure<Item, C1>::type;

        using profile_tag = build_closure_tag;
        using profile_types = profile<
            typeset<Item, Items...>,
            build_closure<typeset<Items...>, Closure>,
            Closure,
            add_to_closure<Item, C1>,
            type>;
    };

        // Helps to debug typesets
        template<typename T1, typename T2>
        struct typeset_compare;

        template<>
        struct typeset_compare<typeset<>, typeset<>>
        {
            using left = typeset<>;
            using right = typeset<>;
        };

        template<typename L, typename...Ls>
        struct typeset_compare<typeset<L, Ls...>, typeset<>>
        {
            using left = typeset<L, Ls...>;
            using right = typeset<>;
        };

        template<typename R, typename...Rs>
        struct typeset_compare<typeset<>, typeset<R, Rs...>>
        {
            using left = typeset<>;
            using right = typeset<R, Rs...>;
        };

        template<typename I, typename... Ls, typename... Rs>
        struct typeset_compare<typeset<I, Ls...>, typeset<I, Rs...>>
        {
            using C = typeset_compare<typeset<Ls...>, typeset<Rs...>>;
            using left = typename C::left;
            using right = typename C::right;
        };

        template<typename L, typename... Ls, typename R, typename... Rs>
        struct typeset_compare<typeset<L, Ls...>, typeset<R, Rs...>>
        {
            using C = typeset_compare<typeset<Ls...>, typeset<Rs...>>;
            using left = typename typeset_ins<L, typename C::left>::type;
            using right = typename typeset_ins<R, typename C::right>::type;
        };

    }

    /*
        Expands a "kernel" (a set of items), into its closure, where each item containing `. X` is expanded into all 
        rules. For this we need a "follows" set as well.
    */
    template<typename Kernel>
    struct closure
    {        
        using T0 = typename impl::build_closure<Kernel, typeset<>>::type;
        // using type = T0;
        using type = typename typeset_sort<T0>::type;

        using T2 = typename closure<type>::type;
        using Cmp = impl::typeset_compare<type, T2>;
        // These might still fail - perhaps drop these tests
        static_assert(type_equals(typename Cmp::left(), typename Cmp::right()), "Closure");
        static_assert(type() == T2(), "Closure error");
        static_assert(type_equals(type(), T2()), "Closure error");

        using profile_tag = closure_tag;
        using profile_types = profile<
            impl::build_closure<Kernel, typeset<>>, 
            typeset_sort<T0>
            >;
    };

}
