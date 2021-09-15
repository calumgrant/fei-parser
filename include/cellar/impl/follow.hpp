#pragma once

#include "rules.hpp"
#include "first.hpp"
#include "sorted_insert.hpp"

namespace cellar
{
    /*
        Constructs the FOLLOW set of a rule, giving the set of possible tokens that could
        follow the next symbol in a rule.

        We need to include the "lookahead" token of the item.
        We need to skip empty rules
    */
    template<typename Item>
    struct follow;

    template<int T1, int T2>
    struct compare<token<T1>, token<T2>>
    {
        static const bool equal = T1 == T2;
        static const bool less = T1 < T2;
    };

    template<typename S, int Id, typename Symbol, int Lookahead>
    struct follow<rule_position<S, rule<Id, Symbol>, 0, Lookahead>>
    {
        using type = typeset<token<Lookahead>>;

        using profile_tag = follow_tag;
        using profile_types = profile_types<rule_position<S, rule<Id, Symbol>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, typename S1, typename... Symbols, int Position, int Lookahead>
    struct follow<rule_position<S, rule<Id, S1, Symbols...>, Position, Lookahead>>
    {
        using type = typename follow<rule_position<S, rule<Id, Symbols...>, Position-1, Lookahead>>::type;

        using profile_tag = follow_tag;
        using profile_types = profile_types<
            rule_position<S, rule<Id, S1, Symbols...>, Position, Lookahead>,
            follow<rule_position<S, rule<Id, Symbols...>, Position-1, Lookahead>>, type>;
    };

    template<typename S, int Id, typename S1, typename... Symbols, int Lookahead>
    struct follow<rule_position<S, rule<Id, S1, Symbols...>, 0, Lookahead>>
    {
        using Next = typename follow<rule_position<S, rule<Id, Symbols...>, 0, Lookahead>>::type;

        using T = typename first<rule<Id, Symbols...>>::type;
        using type = typename type_if<
            potentially_empty_symbol<rule<Id, Symbols...>>::value, 
            typename typeset_sorted_insert<token<Lookahead>, T>::type,
            T>::type;

        using profile_tag = follow_tag;
        using profile_types = profile_types<
            rule_position<S, rule<Id, S1, Symbols...>, 0, Lookahead>,
            follow<rule_position<S, rule<Id, Symbols...>, 0, Lookahead>>,
            first<rule<Id, Symbols...>>, 
            potentially_empty_symbol<rule<Id, Symbols...>>,
            typeset_sorted_insert<token<Lookahead>, T>
            >;
    };

    template<typename S, int Id, int Position, int Lookahead>
    struct follow<rule_position<S, rule<Id>, Position, Lookahead>>
    {
        using type = typeset<>;
        using profile_tag = follow_tag;
        using profile_types = profile_types<rule_position<S, rule<Id>, Position, Lookahead>, type>;
    };
}
