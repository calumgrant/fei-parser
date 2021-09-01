
#pragma once

namespace feiparser
{
    template<typename...Members>
    struct set;

    template<typename Set, typename Item>
    struct insert;

    template<typename Item>
    struct insert<Item, set<>>
    {
        typedef set<Item> type;
    };

    template<typename Item, typename...Members>
    struct insert<Item, set<Item, Members...>>
    {
        typedef set<Members...> type;
    };

    template<typename Item, typename Item2, typename...Members>
    struct insert<Item, set<Item2, Members...>>
    {
    };
}