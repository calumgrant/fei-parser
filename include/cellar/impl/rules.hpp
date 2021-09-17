/*
    Defines the basic elements of rules
*/

#pragma once

#include "utils.hpp"
#include "template_profiler.hpp"
#include "tags.hpp"

namespace cellar
{
    template<int Ch>
    struct ch {};

    template<int...Chs>
    struct chalt {};

    template<int C1, int C2>
    struct chrange {
        static_assert(C1<=C2, "The second character in a chrange is less than the first");
    };

    template<typename...Rules>
    struct seq {};

    template<typename... Rules>
    struct alt
    {
        using rules = alt<Rules...>;
    };

    template<typename Rule>
    struct optional {};

    template<typename Rule>
    struct star {};

    template<typename Rule>
    struct plus {};

    template<typename Rule, int Min, int Max>
    struct repeat {};

    struct reject {};

    template<int Id, typename... Rules>
    struct token
    {
        static const int id = Id;
        using rules = token<Id, Rules...>;
        using profile_tag = token_tag;
        using profile_types = profile<>; // For now, don't profile lexer rules
    };

    template<int Id, typename... Rules>
    struct rule
    {
        static const int id = Id;
        using rules = rule<Id, Rules...>;
        static const int length = sizeof...(Rules);
        using profile_tag = rule_tag;
        using profile_types = profile<Rules...>;
    };

    struct symbol_tag;

    template<typename... Rules>
    struct symbol
    {
        using profile_tag = symbol_tag;
        using profile_types = profile<Rules...>;

        // TODO: Validate the rules
        typedef symbol<Rules...> rules;
    };

    struct empty
    {
        using rules = empty;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    // Unused
    template<int Ch>
    struct utf8ch {};

    template<int... Values> struct string {};

    template<typename Rule, int Group>
    struct capture {};

    // Defines whitespace, which is normally skipped as output from the lexer.
    template<typename Rule>
    struct whitespace {};

    template<int...Chs>
    struct notch {};

    template<typename Symbol, typename Rule, int Position, int Lookahead>
    struct rule_position
    {
        using profile_tag = rule_position_tag;
        using profile_types = profile<Symbol, Rule>;
    };
}
