#pragma once
#include "typeset.hpp"

namespace cellar
{
    template<typename T1, typename T2>
    struct compare
    {
        static const bool equal = false;
        static const bool less = false;
    };

    template<typename T>
    struct compare<T, T>
    {
        static const bool equal = true;
        static const bool less = false;
    };

    template<typename Item, typename Set>
    struct typeset_sorted_insert;

    template<typename Item>
    struct typeset_sorted_insert<Item, typeset<>>
    {
        using type = typeset<Item>;
    };


    template<typename Item, typename Head, typename...Tail>
    struct typeset_sorted_insert<Item, typeset<Head, Tail...>>
    {
        using Cmp = compare<Item, Head>;

        using T1 = typeset<Head, Tail...>;
        using T2 = typeset<Item, Head, Tail...>;
        using T3 = typename typeset_ins<Head, typename typeset_sorted_insert<Item, typeset<Tail...>>::type>::type;

        using type = typename type_if<Cmp::equal, 
            T1,
            typename type_if<Cmp::less, T2, T3>::type>::type; 
    };

    template<typename Typeset>
    struct typeset_sort;

    template<>
    struct typeset_sort<typeset<>>
    {
        using type = typeset<>;
    };

    template<typename Item, typename...Items>
    struct typeset_sort<typeset<Item, Items...>>
    {
        using type = typename typeset_sorted_insert<Item, typename typeset_sort<typeset<Items...>>::type>::type;
        static_assert(typeset_size<type>::value == typeset_size<typeset<Item, Items...>>::value, "Failure in typeset_sort");
    };
}
