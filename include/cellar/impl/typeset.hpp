#pragma once

namespace cellar
{
    template<typename...Members>
    struct typeset;

    template<typename Item, typename Ts>
    struct typeset_contains;

    template<typename Item, typename Ts>
    struct typeset_insert;

    template<typename...Members>
    struct typeset
    {
        constexpr int size() { return sizeof...(Members); }

        constexpr bool empty() { return size()==0; }

        template<typename Item>
        constexpr bool contains(Item)
        {
            return typeset_contains<Item, typeset>::value;
        }

        template<typename Item>
        constexpr auto insert(Item)
        {
            return typeset_insert<Item, typeset>::type();
        }

        using profile_tag = typeset_tag;
        using profile_types = profile<Members...>;
    };

    template<typename Set, typename Item>
    struct typeset_insert;

    template<typename Item>
    struct typeset_insert<Item, typeset<>>
    {
        typedef typeset<Item> type;
        using profile_tag = typeset_insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename...Members>
    struct typeset_insert<Item, typeset<Item, Members...>>
    {
        typedef typeset<Item, Members...> type;

        using profile_tag = typeset_insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename TypeSet> struct typeset_ins {};

    template<typename Item, typename... Members>
    struct typeset_ins<Item, typeset<Members...>>
    {
        typedef typeset<Item, Members...> type;
        using profile_tag = typeset_insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename Item2, typename...Members>
    struct typeset_insert<Item, typeset<Item2, Members...>>
    {
        using S = typename typeset_insert<Item, typeset<Members...>>::type;
        typedef typename typeset_ins<Item2, S>::type type;

        using profile_tag = typeset_insert_tag;
        using profile_types = profile<typeset_insert<Item, typeset<Members...>>, typeset_ins<Item2, S>, type>;
    };

    template<typename T1, typename T2>
    struct typeset_union;

    template<typename T2>
    struct typeset_union<typeset<>, T2>
    {
        typedef T2 type;
    };

    template<typename Item, typename...Members, typename T2>
    struct typeset_union<typeset<Item, Members...>, T2>
    {
        using S = typename typeset_union<typeset<Members...>, T2>::type;
        using type = typename typeset_insert<Item, S>::type;

        using profile_tag = typeset_union_tag;
        using profile_types = profile<type, typeset_union<typeset<Members...>, T2>, typeset_insert<Item, S>>;
    };

    template<typename Ts>
    struct typeset_size;

    template<>
    struct typeset_size<typeset<>>
    {
        static const int value = 0;
        using profile_tag = typeset_size_tag;
        using profile_types = profile<typeset<>>;
    };

    template<typename Item, typename... Members>
    struct typeset_size<typeset<Item, Members...>>
    {
        static const int value = 1 + typeset_size<typeset<Members...>>::value;
        using profile_tag = typeset_size_tag;
        using profile_types = profile<typeset_size<typeset<Members...>>>;
    };

    template<typename Item, typename Ts>
    struct typeset_contains;

    template<typename Item>
    struct typeset_contains<Item, typeset<>>
    {
        static const bool value = false;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<typeset<>>;
    };

    template<typename Item>
    struct typeset_contains<Item, typeset<Item>>
    {
        static const bool value = true;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<Item>>;
    };

    template<typename Item, typename I0>
    struct typeset_contains<Item, typeset<I0>>
    {
        static const bool value = false;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<I0>>;
    };

    template<typename Item, typename I0, typename I1, typename... Items>
    struct typeset_contains<Item, typeset<I0, I1, Items...>>
    {
        static const bool value = typeset_contains<Item, typeset<Items...>>::value;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<I0, I1, Items...>, typeset_contains<Item, typeset<Items...>>>;
    };

    template<typename Item, typename I1, typename... Items>
    struct typeset_contains<Item, typeset<Item, I1, Items...>>
    {
        static const bool value = true;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<Item, I1, Items...>>;
    };

    template<typename Item, typename I0, typename... Items>
    struct typeset_contains<Item, typeset<I0, Item, Items...>>
    {
        static const bool value = true;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<I0, Item, Items...>>;
    };

/*
    template<typename Item, typename I0, typename I1, typename I2, typename...Items>
    struct typeset_contains<Item, typeset<I0, I1, I2>>
    {

    };
*/

    template<
        typename Ts, 
        typename Init,
        template<typename Item, typename Accumulator> typename Aggregate
        >
    struct typeset_aggregate;

    template<
        typename Init,
        template<typename Item, typename Accumulator> typename Aggregate
        >
    struct typeset_aggregate<typeset<>, Init, Aggregate>
    {
        using type = Init;
    };

    template<
        typename Item1,
        typename ...Items,
        typename Init, template<typename Item, typename Accumulator> typename Aggregate
        >
    struct typeset_aggregate<typeset<Item1, Items...>, Init, Aggregate>
    {
        using T0 = typename typeset_aggregate<typeset<Items...>, Init, Aggregate>::type;
        using type = typename Aggregate<Item1, T0>::type;
    };

    struct typeset_subset_tag;

    template<typename Ts1, typename Ts2>
    struct typeset_subset;

    template<typename Ts2>
    struct typeset_subset<typeset<>, Ts2>
    {
        static const bool value = true;
        using profile_tag = typeset_subset_tag;
        using profile_types = profile<typeset<>, Ts2>;
    };

    template<typename Item, typename...Items, typename Ts2>
    struct typeset_subset<typeset<Item, Items...>, Ts2>
    {
        static const bool value = typeset_contains<Item, Ts2>::value &&
            typeset_subset<typeset<Items...>, Ts2>::value;

        using profile_tag = typeset_subset_tag;
        using profile_types = profile<
            typeset<Item, Items...>,
            Ts2,
            typeset_contains<Item, Ts2>,
            typeset_subset<typeset<Items...>, Ts2>>;
    };

    template<typename Ts1, typename Ts2>
    struct typeset_equals
    {
        static const bool value = typeset_subset<Ts1, Ts2>::value && typeset_subset<Ts2, Ts1>::value;
        using profile_tag = no_tag;
        using profile_types = profile<Ts1, Ts2, typeset_subset<Ts1, Ts2>, typeset_subset<Ts2, Ts1>>;
    };

    constexpr typeset<> empty_ts() { return typeset<>(); }

    constexpr bool is_empty(typeset<>) { return true; }

    template<typename Item, typename...Items>
    constexpr bool is_empty(typeset<Item, Items...>) { return false; }

    template<typename... Ts1, typename... Ts2>
    constexpr bool operator<=(typeset<Ts1...> t1, typeset<Ts2...> t2)
    {
        return typeset_subset<typeset<Ts1...>, typeset<Ts2...>>::value;
    }

    template<typename... Ts1, typename... Ts2>
    constexpr bool operator==(typeset<Ts1...> t1, typeset<Ts2...> t2)
    {
        return t1<=t2 && t2<=t1;
    }

    template<typename... Ts1, typename... Ts2>
    constexpr auto operator+(typeset<Ts1...> t1, typeset<Ts2...> t2)
    {
        return typeset_union<typeset<Ts1...>, typeset<Ts2...>>::type();
    }

    template<typename T1, typename T2>
    constexpr bool type_equals(T1, T2) { return false; }

    template<typename T>
    constexpr bool type_equals(T a, T b) { return true; }
}
