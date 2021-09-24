#pragma once

#include "rules.hpp"
#include "first.hpp"

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

    template<typename S, int Id, typename Symbol, int Lookahead>
    struct follow<rule_position<S, rule<Id, Symbol>, 0, Lookahead>>
    {
        using type = token<Lookahead>;

        using profile_tag = follow_tag;
        using profile_types = profile<rule_position<S, rule<Id, Symbol>, 0, Lookahead>, type>;
    };

    template<typename S, int Id, typename S1, typename... Symbols, int Position, int Lookahead>
    struct follow<rule_position<S, rule<Id, S1, Symbols...>, Position, Lookahead>>
    {
        using type = typename follow<rule_position<S, rule<Id, Symbols...>, Position-1, Lookahead>>::type;

        using profile_tag = follow_tag;
        using profile_types = profile<
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
            typename insert<token<Lookahead>, T>::type,
            T>::type;

        using profile_tag = follow_tag;
        using profile_types = profile<
            rule_position<S, rule<Id, S1, Symbols...>, 0, Lookahead>,
            follow<rule_position<S, rule<Id, Symbols...>, 0, Lookahead>>,
            first<rule<Id, Symbols...>>, 
            potentially_empty_symbol<rule<Id, Symbols...>>,
            insert<token<Lookahead>, T>
            >;
    };

    template<typename S, int Id, int Position, int Lookahead>
    struct follow<rule_position<S, rule<Id>, Position, Lookahead>>
    {
        using type = empty_tree;
        using profile_tag = follow_tag;
        using profile_types = profile<rule_position<S, rule<Id>, Position, Lookahead>, type>;
    };
}
