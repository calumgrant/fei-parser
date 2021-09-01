#pragma once
#include "normalize.hpp"

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

    template<int C1, int C2, int Ch>
    struct next<chrange<C1,C2>, Ch>
    {
        typedef typename type_if<C1<=Ch && Ch<=C2, empty, reject>::type type;
    };

    template<int C>
    struct next<ch<C>, C>
    {
        typedef empty type;
    };

    template<int C1, int C2>
    struct next<ch<C1>, C2>
    {
        typedef reject type;
    };

    template<typename T1, typename T2, int Ch>
    struct next<alt<T1,T2>, Ch>
    {
        typedef typename normalize<T1>::type S1;
        typedef typename normalize<T2>::type S2;
        typedef alt<typename next<S1,Ch>::type, typename next<S2,Ch>::type> t;
        typedef typename normalize<t>::type type;
    };

    template<typename T1, typename T2, int Ch>
    struct next<seq<T1,T2>, Ch>
    {
        // If T1 is not empty, then this is the answer
        typedef seq<typename next<T1, Ch>::type, T2> t1;

        // If T1 can be empty, then this is also the answer
        typedef typename next<T2, Ch>::type t2;
        typedef alt<t1, t2> t3;
        typedef typename type_if<accepts<T1>::value, t3, t1>::type t4;
        typedef typename normalize<t4>::type type;
    };

    template<typename T, int Ch>
    struct next<star<T>, Ch>
    {
        typedef seq<typename next<T, Ch>::type, star<T>> U;
        typedef typename normalize<U>::type type;
    };

    template<int Tok, typename R, int Ch>
    struct next<token<Tok, R>, Ch>
    {
        typedef token<Tok, typename next<R, Ch>::type> type;
    };

    template<int Ch>
    struct next<notch<>, Ch>
    {
        typedef empty type;
    };

    template<int Ch, int...Chs>
    struct next<notch<Ch, Chs...>, Ch>
    {
        typedef reject type;
    };

    template<int C1, int C2, int...Chs>
    struct next<notch<C1, Chs...>, C2>
    {
        typedef typename next<notch<Chs...>, C2>::type type;
    };
}
