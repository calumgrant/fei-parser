/*
    Defines the basic elements of rules
*/

#pragma once

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
    };

    template<int Id, typename... Rules>
    struct rule
    {
        using rules = rule<Id, Rules...>;
    };

    template<typename... Rules>
    struct symbol
    {
        // TODO: Validate the rules
        typedef symbol<Rules...> rules;
    };

    struct empty
    {
        using rules = empty;
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

    template<typename Rule, int Position, int Lookahead>
    struct rule_position {};
}
