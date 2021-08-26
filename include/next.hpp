#pragma once

namespace feiparser
{
  template<typename Rule, int Ch>
    struct next;

    template<int Ch>
    struct next<reject, Ch>
    {
        typedef reject type;
    };

    template<int Ch>
    struct next<empty, Ch>
    {
        typedef reject type;
    };

    template<bool B, typename T, typename F>
    struct type_if;

    template<typename T, typename F>
    struct type_if<true, T, F>
    {
        typedef T type;
    };

    template<typename T, typename F>
    struct type_if<false, T, F>
    {
        typedef F type;
    };

    template<int C1, int C2, int Ch>
    struct next<chrange<C1,C2>, Ch>
    {
        typedef typename type_if<C1<=Ch && Ch<=C2, empty, reject>::type type;
    };

    template<typename T1, typename T2, int Ch>
    struct next<alt<T1,T2>, Ch>
    {
        typedef alt<typename next<T1,Ch>::type, typename next<T2,Ch>::type> t;
        typedef typename simplify<t>::type type;
    };
}