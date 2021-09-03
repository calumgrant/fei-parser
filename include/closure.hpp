#pragma once

#include "typeset.hpp"

namespace feiparser
{
    template<typename Set>
    struct closure
    {
        using type = Set;
    };

    template<>
    struct closure<typeset<>>
    {
        using type = typeset<>;
    };

    template<typename RulePosition, typename Closure, bool AlreadyContains = typeset_contains<RulePosition, Closure>::value>
    struct build_closure;

    template<typename RulePosition, typename Closure>
    struct build_closure<RulePosition, Closure, true>
    {
        using type = Closure;
    };

    template<typename Rule, int Position, int Lookahead, typename Closure>
    struct build_closure<rule_position<Rule, Position, Lookahead>, Closure, false>
    {
        using type = typename typeset_ins<rule_position<Rule, Position, Lookahead>, Closure>::type;
    };

    template<typename Rule, typename... Rules>
    struct closure<typeset<Rule, Rules...>>
    {
        using C2 = typename closure<typeset<Rules...>>::type;
        using type = typename build_closure<Rule, C2>::type;
    };

    // DELETEME:
    template<typename R1, typename R2>
    struct closure<alt<R1, R2>>
    {
        using S1 = typename closure<R1>::type;
        using S2 = typename closure<R2>::type;
        using type = typename typeset_union<S1,S2>::type;
    };

    template<typename Rule, int Position, int Lookahead>
    struct closure<rule_position<Rule, Position, Lookahead>>
    {
        // Look at the symbol after the dot


    };

}