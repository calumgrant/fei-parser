#pragma once

namespace feiparser
{
    template<int C>
    struct ch {};

    template<int C1, int C2>
    struct range {};

    template<typename...Rules>
    struct seq {};

    template<typename... Rules>
    struct or {};

    template<typename Rule>
    struct optional {};

    template<typename Rule>
    struct star {};

    template<typename Rule>
    struct plus {};

    template<typename Rule, int Min, int Max>
    struct repeat {};
}