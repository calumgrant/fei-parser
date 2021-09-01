#pragma once

#include "typeset.hpp"

namespace feiparser
{
    template<typename Set>
    struct closure;

    template<>
    struct closure<typeset<>>
    {
        typedef typeset<> type;
    };

    template<typename Rule, typename... Rules>
    struct closure<typeset<Rule, Rules...>>
    {
        using C1 = closure<Rule>::type;
        using C2 = closure<typeset<Rules...>>::type;
        using type = typeset_union<C1, C2>::type;
    };

    template<typename Rules
}