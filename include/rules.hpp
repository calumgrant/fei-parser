/*
    Defines the basic elements of rules
*/

#pragma once

namespace feiparser
{
    template<int Ch>
    struct ch {};

    template<int...Chs>
    struct chseq {};

    template<int...Chs>
    struct chalt {};

    template<int C1, int C2>
    struct chrange {};

    template<typename...Rules>
    struct seq {};

    template<typename... Rules>
    struct alt {};

    template<typename Rule>
    struct optional {};

    template<typename Rule>
    struct star {};

    template<typename Rule>
    struct plus {};

    template<typename Rule, int Min, int Max>
    struct repeat {};

    struct reject {};

    template<typename Rule, int Value>
    struct accept {};

    struct empty {};

    template<int Ch>
    struct utf8ch {};

    template<char... Values> struct string {};

    template<typename Rule, int Group>
    struct capture {};

    // Defines whitespace, which is normally skipped as output from the lexer.
    template<typename Rule>
    struct whitespace {};
}
