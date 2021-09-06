
#pragma once

namespace feiparser
{
    template<typename...Members>
    struct typeset {};

    template<typename Set, typename Item>
    struct typeset_insert;

    template<typename Item>
    struct typeset_insert<Item, typeset<>>
    {
        typedef typeset<Item> type;
    };

    template<typename Item, typename...Members>
    struct typeset_insert<Item, typeset<Item, Members...>>
    {
        typedef typeset<Item, Members...> type;
    };

    template<typename Item, typename TypeSet> struct typeset_ins {};

    template<typename Item, typename... Members>
    struct typeset_ins<Item, typeset<Members...>>
    {
        typedef typeset<Item, Members...> type;
    };

    template<typename Item, typename Item2, typename...Members>
    struct typeset_insert<Item, typeset<Item2, Members...>>
    {
        using S = typename typeset_insert<Item, typeset<Members...>>::type;
        typedef typename typeset_ins<Item2, S>::type type;
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
    };

    template<typename Ts>
    struct typeset_size;

    template<>
    struct typeset_size<typeset<>>
    {
        static const int value = 0;
    };

    template<typename Item, typename... Members>
    struct typeset_size<typeset<Item, Members...>>
    {
        static const int value = 1 + typeset_size<typeset<Members...>>::value;
    };

    template<typename Item, typename Ts>
    struct typeset_contains;

    template<typename Item>
    struct typeset_contains<Item, typeset<>>
    {
        static const bool value = false;
    };

    template<typename Item, typename... Members>
    struct typeset_contains<Item, typeset<Item, Members...>>
    {
        static const bool value = true;
    };

    template<typename Item1, typename Item2, typename... Members>
    struct typeset_contains<Item1, typeset<Item2, Members...>>
    {
        static const bool value = typeset_contains<Item1, typeset<Members...>>::value;
    };

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

    template<typename Ts1, typename Ts2>
    struct typeset_subset;

    template<typename Ts2>
    struct typeset_subset<typeset<>, Ts2>
    {
        static const bool value = true;
    };

    template<typename Item, typename...Items, typename Ts2>
    struct typeset_subset<typeset<Item, Items...>, Ts2>
    {
        static const bool value = typeset_contains<Item, Ts2>::value &&
            typeset_subset<typeset<Items...>, Ts2>::value;
    };

    template<typename Ts1, typename Ts2>
    struct typeset_equals
    {
        static const bool value = typeset_subset<Ts1, Ts2>::value && typeset_subset<Ts2, Ts1>::value;
    };

}
