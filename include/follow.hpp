#pragma once

#include "rules.hpp"

namespace feiparser
{
    /*
        Constructs the FOLLOW set of a rule, giving the set of possible tokens that could
        follow the next symbol in a rule.

        We need to include the "lookahead" token of the item.
        We need to skip empty rules
    */
    template<typename Item>
    struct follow;

    template<int Id, typename Symbol, int Lookahead>
    struct follow<rule_position<rule<Id, Symbol>, 0, Lookahead>>
    {
        using type = typeset<token<Lookahead>>;
    };

    template<int Id, typename S1, typename S2, typename... Symbols, int Position, int Lookahead>
    struct follow<rule_position<rule<Id, S1, S2, Symbols...>, Position, Lookahead>>
    {
        using type = typename follow<rule<Id, Symbol...>, Position-1, Lookahead>::type;
    };

    template<int Id, typename Symbol, typename... Symbols, int Lookahead>
    struct follow<rule_position<rule<Id, Symbol, Symbols...>, 0, Lookahead>>
    {
        using Next = typename follow<rule_position<rule<Id, Symbols...>, 0, Lookahead>>::type;

        using T = typename first<rule<Id, Symbols...>::type;

        using U = typename typeset_union<T
        // If empty, use the next set as well

    };

}