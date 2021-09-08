#pragma once

#include "typeset.hpp"
#include "potentially_empty_symbol.hpp"
#include "follow.hpp"

namespace cellar
{
    /*
        Finds the next symbol in an Item (the symbol to the right of the `.`)
    */
    template<typename Item>
    struct getnext;

    template<typename S, int Id, int Lookahead, typename Symbol, typename...Symbols>
    struct getnext<rule_position<S, rule<Id, Symbol, Symbols...>, 0, Lookahead>>
    {
        using type = Symbol;
        using type_or_lookahead = type;
    };

    template<typename S, int Id, int Position, int Lookahead, typename Symbol, typename...Symbols>
    struct getnext<rule_position<S, rule<Id, Symbol, Symbols...>, Position, Lookahead>>
    {
        using type = typename getnext<rule_position<S, rule<Id, Symbols...>, Position-1, Lookahead>>::type;
        using type_or_lookahead = type;
    };

    template<typename S, int Id, int Position, int Lookahead>
    struct getnext<rule_position<S, rule<Id>, Position, Lookahead>>
    {
        using type = empty;
    };

    template<typename S, int Id, int Lookahead>
    struct getnext<rule_position<S, rule<Id>, 0, Lookahead>>
    {
        using type = empty;
        using type_or_lookahead = token<Lookahead>;
    };


    template<typename Symbol, typename Closure>
    struct expand_next;

    template<typename S, typename Item, typename Follows, typename Closure>
    struct expand_item
    {
        using type = Closure;
    };

    template<typename S, typename Item, typename...Items, typename Follows, typename Closure>
    struct expand_item<S, symbol<Item, Items...>, Follows, Closure>
    {
        using type = Closure;
    };

    template<typename S, int Id, typename...Rule, typename...Items, typename Follows, typename Closure>
    struct expand_item<S, symbol<token<Id, Rule...>, Items...>, Follows, Closure>
    {
        using T = rule<Id, token<Id, Rule...>>;
        using type = typename expand_item<S, symbol<T, Items...>, Follows, Closure>::type;
    };

    template<typename S, typename...Items1, typename...Items2, typename Follows, typename Closure>
    struct expand_item<S, symbol<symbol<Items1...>, Items2...>, Follows, Closure>
    {
        using T = typename expand_item<S, symbol<Items1...>, Follows, Closure>::type;
        using type = typename expand_item<S, symbol<T, Items2...>, Follows, T>::type;
    };

    template<typename Symbol, typename Rule, typename Follows, typename Closure>
    struct add_items;

    template<typename S, typename Rule, typename Closure>
    struct add_items<S, Rule, typeset<>, Closure>
    {
        using type = Closure;
    };

    template<typename Item>
    struct item;

    template<typename Item, typename Closure, bool NeedToExpand = !typeset_contains<Item, Closure>::value && item<Item>::reads_symbol>
    struct add_to_closure;

    template<typename S, typename Rule, int Lookahead, typename...Follows, typename Closure>
    struct add_items<S, Rule, typeset<token<Lookahead>, Follows...>, Closure>
    {
        using Item = rule_position<S, Rule, 0, Lookahead>;
        using type = typename add_to_closure<Item, Closure>::type;
    };

    template<typename T>
    struct Debug
    {
        using Break = typename T::fail;
    };

    template<typename S, int Id, typename...Rule, typename...Items, typename Follows, typename Closure>
    struct expand_item<S, symbol<rule<Id, Rule...>, Items...>, Follows, Closure>
    {
        using C0 = typename add_items<S, rule<Id, Rule...>, Follows, Closure>::type;
        using type = typename expand_item<S, symbol<Items...>, Follows, C0>::type;
    };

    template<typename Item, typename Closure>
    struct add_to_closure<Item, Closure, false>
    {
        using type = typename typeset_insert<Item, Closure>::type;
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

    template<typename Item>
    struct skip_symbol;

    template<typename S, typename Rule, int Position, int Lookahead>
    struct skip_symbol<rule_position<S, Rule, Position, Lookahead>>
    {
        using type = rule_position<S, Rule, Position+1, Lookahead>;
    };
    
    template<typename Item>
    struct item;

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
        using C1 = typename typeset_insert<Item, Closure>::type;

        using Follows = typename follow<Item>::type;

        using NextSymbol= typename getnext<Item>::type;

        using C2 = typename type_if<
            potentially_empty_symbol<NextSymbol>::value,
            typename add_to_closure<typename skip_symbol<Item>::type, C1>::type,
            C1>::type;

        using type = typename expand_item<NextSymbol, typename NextSymbol::rules, Follows, C2>::type;
    };


    template<typename Closure, typename Added>
    struct build_closure;

    template<typename Closure>
    struct build_closure<typeset<>, Closure>
    {
        using type = Closure;
    };

    template<typename Item, typename...Items, typename Closure>
    struct build_closure<typeset<Item, Items...>, Closure>
    {
        using C1 = typename build_closure<typeset<Items...>, Closure>::type;
        using type = typename add_to_closure<Item, C1>::type;
    };

    /*
        Expands a "kernel" (a set of items), into its closure, where each item containing `. X` is expanded into all 
        rules. For this we need a "follows" set as well.
    */
    template<typename Kernel>
    struct closure
    {
        using type = typename build_closure<Kernel, typeset<>>::type;
    };
}
