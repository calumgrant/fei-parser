#pragma once

#include "typeset.hpp"
#include "potentially_empty_symbol.hpp"

namespace feiparser
{
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

    template<typename Item, typename Closure>
    struct expand_item
    {
        using S = typename getnext<Item>::type;

        // If S is a symbol, add all members of the symbol

        using type = Closure;
    };

    template<typename Item, typename Closure, bool Recursive = typeset_contains<Item, Closure>::value>
    struct add_to_closure;

    template<typename Item, typename Closure>
    struct add_to_closure<Item, Closure, true>
    {
        using type = Closure;
    };

    template<typename Item, typename Closure>
    struct add_to_closure<Item, Closure, false>
    {
        // Insert Item into Closure
        using C = typename typeset_insert<Item, Closure>::type;

        // What is the next symbol in Item?
        // It could be:
        // a token (ignore)
        // a non-empty symbol or class
        // an empty symbol or class

        using NextSymbol = typename getnext<Item>::type;

        static const bool empty = potentially_empty_symbol<NextSymbol>::value;

        using type = typename expand_item<Item, Closure>::type;
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

    template<typename Kernel>
    struct closure
    {
        using type = typename build_closure<Kernel, typeset<>>::type;
    };
}