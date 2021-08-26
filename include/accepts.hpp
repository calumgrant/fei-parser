#pragma once
#include "rules.hpp"

namespace feiparser
{
    static const int Match = -1;
    static const int NoMatch = -2;

    template<typename Rule>
    struct accepts;

    template<>
    struct accepts<reject>
    {
        static const bool value = false;
        static const int token = NoMatch;
    };

    template<>
    struct accepts<empty>
    {
        static const bool value = true;
        static const int token = Match;
    };

    template<typename Rule, int Value>
    struct accepts<accept<Rule, Value>>
    {
        static const bool value = accepts<Rule>::value;
        static const int token = Value;
    };

    template<int C1, int C2>
    struct accepts<chrange<C1,C2>>
    {
        static const bool value = false;
        static const int token = NoMatch;
    };

    template<int C>
    struct accepts<ch<C>>
    {
        static const bool value = false;
        static const int token = NoMatch;
    };

    template<typename T1, typename T2>
    struct accepts<alt<T1,T2>>
    {
        static const bool value = accepts<T1>::value || accepts<T2>::value;
        static const int token = accepts<T1>::value && accepts<T2>::value ? 
            (accepts<T1>::token < accepts<T2>::token ? accepts<T1>::token : accepts<T2>::token) :
            accepts<T1>::value ? accepts<T1>::token : accepts<T2>::token;
    };

    template<typename T1, typename T2>
    struct accepts<seq<T1,T2>>
    {
        static const bool value = accepts<T1>::value;
        static const int token = accepts<T2>::value && accepts<T2>::token < accepts<T1>::token ? accepts<T2>::token : accepts<T1>::token;
    };

    template<typename T>
    struct accepts<star<T>>
    {
        static const bool value = true;
        static const bool token = Match;
    };
}
