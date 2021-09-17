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

/*
    template<typename T>
    struct compare<T, T>
    {
        static const bool equal = true;
        static const bool less = false;
    };
    */

    struct sorted_insert_tag;

    template<typename Item, typename Set>
    struct typeset_sorted_insert;

    template<typename Item>
    struct typeset_sorted_insert<Item, typeset<>>
    {
        using type = typeset<Item>;
        using profile_tag = typeset_sorted_insert_tag;
        using profile_types = profile<Item, typeset<>>;
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

        using profile_tag = typeset_sorted_insert_tag;
        using profile_types = profile<Item, typeset<Head, Tail...>, T1, T2, T3>;
    };

    template<typename Item, typename...Tail>
    struct typeset_sorted_insert<Item, typeset<Item, Tail...>>
    {
        using type = typeset<Item, Tail...>;

        using profile_tag = typeset_sorted_insert_tag;
        using profile_types = profile<Item, typeset<Item, Tail...>>;
    };

    template<typename Typeset>
    struct typeset_sort;

    template<>
    struct typeset_sort<typeset<>>
    {
        using type = typeset<>;
        using profile_tag = typeset_sort_tag;
        using profile_types = profile<typeset<>>;
    };

    template<typename Item, typename...Items>
    struct typeset_sort<typeset<Item, Items...>>
    {
        using type = typename typeset_sorted_insert<Item, typename typeset_sort<typeset<Items...>>::type>::type;
        static_assert(typeset_size<type>::value == typeset_size<typeset<Item, Items...>>::value, "Failure in typeset_sort");

        using profile_tag = typeset_sort_tag;
        using profile_types = profile<typeset_sort<typeset<Items...>>, typeset_sorted_insert<Item, typename typeset_sort<typeset<Items...>>::type>, type>;
    };

    template<typename T1, typename T2>
    struct typeset_sorted_union;

    template<typename T2>
    struct typeset_sorted_union<typeset<>, T2>
    {
        typedef T2 type;
        using profile_tag = typeset_sorted_union_tag;
        using profile_types = profile<typeset<>, T2>;
    };

    template<typename Item, typename...Members, typename T2>
    struct typeset_sorted_union<typeset<Item, Members...>, T2>
    {
        using S = typename typeset_sorted_union<typeset<Members...>, T2>::type;
        using type = typename typeset_sorted_insert<Item, S>::type;

        using profile_tag = typeset_sorted_union_tag;
        using profile_types = profile<
            typeset<Item, Members...>,
            T2,
            typeset_sorted_union<typeset<Members...>, T2>,
            typeset_sorted_insert<Item, S>>;
    };
}
