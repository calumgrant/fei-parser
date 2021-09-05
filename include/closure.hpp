#pragma once

#include "typeset.hpp"
#include "potentially_empty_symbol.hpp"
#include "follow.hpp"

namespace feiparser
{
    /*
        Finds the next symbol in an Item (the symbol to the right of the `.`)
    */
    template<typename Item>
    struct getnext;

    template<int Id, int Lookahead, typename Symbol, typename...Symbols>
    struct getnext<rule_position<rule<Id, Symbol, Symbols...>, 0, Lookahead>>
    {
        using type = Symbol;
        using type_or_lookahead = type;
    };

    template<int Id, int Position, int Lookahead, typename Symbol, typename...Symbols>
    struct getnext<rule_position<rule<Id, Symbol, Symbols...>, Position, Lookahead>>
    {
        using type = typename getnext<rule_position<rule<Id, Symbols...>, Position-1, Lookahead>>::type;
        using type_or_lookahead = type;
    };

    template<int Id, int Lookahead>
    struct getnext<rule_position<rule<Id>, 0, Lookahead>>
    {
        using type = empty;
        using type_or_lookahead = token<Lookahead>;
    };


    template<typename Symbol, typename Closure>
    struct expand_next;

    template<typename Item, typename Follows, typename Closure>
    struct expand_item
    {
        using type = Closure;
    };

    template<typename Item, typename...Items, typename Follows, typename Closure>
    struct expand_item<symbol<Item, Items...>, Follows, Closure>
    {
        using type = Closure;
    };

    template<int Id, typename...Rule, typename...Items, typename Follows, typename Closure>
    struct expand_item<symbol<token<Id, Rule...>, Items...>, Follows, Closure>
    {
        using type = typename expand_item<symbol<Items...>, Follows, Closure>::type;
    };

    template<typename Rule, typename Follows, typename Closure>
    struct add_items;

    template<typename Rule, typename Closure>
    struct add_items<Rule, typeset<>, Closure>
    {
        using type = Closure;
    };

    template<typename Item, typename Closure, bool Recursive = typeset_contains<Item, Closure>::value>
    struct add_to_closure;

    template<typename Rule, int Lookahead, typename...Follows, typename Closure>
    struct add_items<Rule, typeset<token<Lookahead>, Follows...>, Closure>
    {
        using Item = rule_position<Rule, 0, Lookahead>;
        using type = typename add_to_closure<Item, Closure>::type;
    };

    template<int Id, typename...Rule, typename...Items, typename Follows, typename Closure>
    struct expand_item<symbol<rule<Id, Rule...>, Items...>, Follows, Closure>
    {
        // using T0 = rule_position
        using C0 = typename add_items<rule<Id, Rule...>, Follows, Closure>::type;
        using type = typename expand_item<symbol<Items...>, Follows, C0>::type;
    };

    template<typename Item, typename Closure>
    struct add_to_closure<Item, Closure, true>
    {
        using type = Closure;
    };

    template<typename Symbol>
    struct is_terminal;

    template<int Id, typename...X>
    struct is_terminal<token<Id, X...>>
    {
        static const bool value = false;
    };

    template<typename...X>
    struct is_terminal<symbol<X...>>
    {
        static const bool value = true;
    };

    template<typename Item>
    struct skip_symbol;

    template<typename Rule, int Position, int Lookahead>
    struct skip_symbol<rule_position<Rule, Position, Lookahead>>
    {
        using type = rule_position<Rule, Position+1, Lookahead>;
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
    struct add_to_closure<Item, Closure, false>
    {
        // Insert Item into Closure
        using C1 = typename typeset_insert<Item, Closure>::type;

        using Follows = typename follow<Item>::type;

        using NextSymbol= typename getnext<Item>::type;
        static const bool needToExpand = !is_terminal<typename NextSymbol::rule>::value;

        using C2 = typename type_if<
            potentially_empty_symbol<Item>::value,
            typename add_to_closure<typename skip_symbol<Item>::type, C1>::type,
            C1>::type;

        using C3 = typename expand_item<Item, Follows, C2>::type;

        using type = typename type_if<needToExpand, C3, C2>::type;
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
        using C0 = typename typeset_insert<Item, Closure>::type;
        using C1 = typename build_closure<typeset<Items...>, C0>::type;
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