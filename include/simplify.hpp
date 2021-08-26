#pragma once

/*
    simplify<> rewrites rules into a more simple form.
    This is particularly true of "synthesised" rules (for example, those rules
    produced by next<>).

    simplify<> also desugars more complex rules into simpler elements. For example,
    string<A,B,C> gets simplified to seq<ch<A>, seq<ch<B>, ch<C>>>
*/

// !! Rename this to normalize

namespace feiparser
{
    // Rewrites lexer rules to a canonical form.
    template<typename Rule>
    struct simplify;

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

    // The base types do not simplify
    template<int Ch>
    struct simplify<ch<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<>
    struct simplify<string<>>
    {
        typedef empty type;
    };

    template<int Ch>
    struct simplify<string<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<int C1, int C2, int...Cs>
    struct simplify<string<C1, C2, Cs...>>
    {
        typedef seq<ch<C1>, typename simplify<string<C2, Cs...>>::type> type;
    };

    template<typename T1, typename T2, typename T3, typename... T4>
    struct simplify<seq<T1, T2, T3, T4...>>
    {
        typedef typename simplify<seq<T1, seq<T2, T3, T4...>>>::type type; 
    };

    template<typename T1, typename T2, typename T3, typename... T4>
    struct simplify<alt<T1, T2, T3, T4...>>
    {
        typedef typename simplify<alt<T1, seq<T2, T3, T4...>>>::type type; 
    };

    template<typename T1, typename T2, typename T3>
    struct simplify<alt<alt<T1,T2>,T3>>
    {
        typedef typename simplify<alt<T1,alt<T2,T3>>>::type type;
    };

    template<typename T1, typename T2>
    struct simplify<seq<T1,T2>>
    {
        typedef seq<typename simplify<T1>::type, typename simplify<T2>::type> type;
    };

    template<typename T>
    struct simplify<alt<reject, T>>
    {
        typedef typename simplify<T>::type type;
    };

    template<typename T>
    struct simplify<alt<T, reject>>
    {
        typedef typename simplify<T>::type type;
    };

    template<>
    struct simplify<alt<reject, reject>>
    {
        typedef reject type;
    };

    template<>
    struct simplify<chalt<>>
    {
        typedef reject type;
    };

    template<>
    struct simplify<empty>
    {
        typedef empty type;
    };


    template<int Ch>
    struct simplify<chalt<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<int Ch, int...Chs>
    struct simplify<chalt<Ch, Chs...>>
    {
        typedef alt<ch<Ch>, typename simplify<chalt<Chs...>>::type> type;
    };

    template<int Ch>
    struct simplify<chrange<Ch,Ch>>
    {
        typedef ch<Ch> type;
    };

    template<int Ch1, int Ch2>
    struct simplify<chrange<Ch1,Ch2>>
    {
        typedef chrange<Ch1, Ch2> type;
    };

    // Simplifying seq

    template<typename T>
    struct simplify<seq<empty, T>>
    {
        typedef typename simplify<T>::type type;
    };

    template<>
    struct simplify<reject>
    {
        typedef reject type;
    };

    template<typename T>
    struct simplify<seq<reject, T>>
    {
        typedef reject type;
    };

    template<typename Rule>
    struct simplify<optional<Rule>>
    {
        typedef alt<empty, typename simplify<Rule>::type> type;
    };

    template<typename Rule>
    struct simplify<plus<Rule>>
    {
        typedef typename simplify<Rule>::type S;

        typedef seq<S, star<S>> type;
    };

    template<typename Rule>
    struct simplify<star<Rule>>
    {
        typedef typename simplify<Rule>::type S;
        typedef star<S> type;
    };

    template<>
    struct simplify<alt<>>
    {
        typedef reject type;
    };

    template<typename R>
    struct simplify<alt<R>>
    {
        typedef typename simplify<R>::type type;
    };

    template<typename Rule>
    struct make_nonempty
    {
        static const bool empty = false;
        typedef Rule type;
    };

    template<typename T>
    struct make_nonempty<alt<empty, T>>
    {
        typedef typename make_nonempty<T>::type type;
    };

    template<typename T>
    struct make_nonempty<alt<T, empty>>
    {
        typedef typename make_nonempty<T>::type type;
    };

    template<typename T>
    struct simplify<alt<T,T>>
    {
        typedef typename simplify<T>::type type;
    };

    template<typename T1, typename T2>
    struct make_nonempty<seq<T1,T2>>
    {
        typedef typename make_nonempty<T1>::type n1;
        typedef typename make_nonempty<T2>::type n2;
        typedef alt<seq<T1, n2>, seq<n1, T2>> type;
    };

    template<typename T1, typename T2>
    struct make_nonempty<alt<T1,T2>>
    {
        typedef make_nonempty<T1> n1;
        typedef make_nonempty<T2> n2;
        typedef alt<typename n1::type, typename n2::type> type;
        static const bool empty = n1::empty || n2::empty;
    };

    template<typename T1, typename T2>
    struct simplify<alt<T1,T2>>
    {
        typedef make_nonempty<T1> n1;
        typedef make_nonempty<T2> n2;
        typedef alt<typename n1::type, typename n2::type> t1;
//         typedef typename simplify<t1>::type t2;

        typedef typename type_if<n1::empty || n2::empty, alt<empty, t1>, t1>::type type;
    };
}
