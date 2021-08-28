#pragma once

/*
    normalize<> rewrites rules into a more simple form.
    This is particularly true of "synthesised" rules (for example, those rules
    produced by next<>).

    normalize<> also desugars more complex rules into simpler elements. For example,
    string<A,B,C> gets simplified to seq<ch<A>, seq<ch<B>, ch<C>>>
*/

// !! Rename this to normalize

namespace feiparser
{
    // Rewrites lexer rules to a canonical form.
    template<typename Rule>
    struct normalize;

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

    // The base types do not normalize
    template<int Ch>
    struct normalize<ch<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<>
    struct normalize<string<>>
    {
        typedef empty type;
    };

    template<int Ch>
    struct normalize<string<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<int C1, int C2, int...Cs>
    struct normalize<string<C1, C2, Cs...>>
    {
        typedef seq<ch<C1>, typename normalize<string<C2, Cs...>>::type> type;
    };

    template<typename T1, typename T2, typename T3, typename... T4>
    struct normalize<seq<T1, T2, T3, T4...>>
    {
        typedef typename normalize<seq<T1, seq<T2, T3, T4...>>>::type type; 
    };

    template<typename T1, typename T2, typename T3, typename... T4>
    struct normalize<alt<T1, T2, T3, T4...>>
    {
        typedef typename normalize<alt<T1, alt<T2, T3, T4...>>>::type type; 
    };

    template<typename T1, typename T2, typename T3>
    struct normalize<alt<alt<T1,T2>,T3>>
    {
        typedef typename normalize<alt<T1,alt<T2,T3>>>::type type;
    };

    template<typename T1, typename T2>
    struct normalize<alt<alt<T1,T2>,reject>>
    {
        typedef typename normalize<alt<T1,T2>>::type type;
    };


    template<typename T1, typename T2>
    struct normalize<seq<T1,T2>>
    {
        typedef seq<typename normalize<T1>::type, typename normalize<T2>::type> type;
    };

    template<typename T>
    struct normalize<alt<reject, T>>
    {
        typedef typename normalize<T>::type type;
    };

    template<typename T>
    struct normalize<alt<T, reject>>
    {
        typedef typename normalize<T>::type type;
    };

    template<>
    struct normalize<alt<empty, reject>>
    {
        typedef empty type;
    };

    template<>
    struct normalize<alt<reject, empty>>
    {
        typedef empty type;
    };

    template<>
    struct normalize<alt<reject, reject>>
    {
        typedef reject type;
    };

    template<>
    struct normalize<chalt<>>
    {
        typedef reject type;
    };

    template<>
    struct normalize<empty>
    {
        typedef empty type;
    };


    template<int Ch>
    struct normalize<chalt<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<int Ch, int...Chs>
    struct normalize<chalt<Ch, Chs...>>
    {
        typedef alt<ch<Ch>, typename normalize<chalt<Chs...>>::type> type;
    };

    template<int Ch>
    struct normalize<chrange<Ch,Ch>>
    {
        typedef ch<Ch> type;
    };

    template<int Ch1, int Ch2>
    struct normalize<chrange<Ch1,Ch2>>
    {
        typedef chrange<Ch1, Ch2> type;
    };

    // normalizeing seq

    template<typename T>
    struct normalize<seq<empty, T>>
    {
        typedef typename normalize<T>::type type;
    };

    template<>
    struct normalize<reject>
    {
        typedef reject type;
    };

    template<typename T>
    struct normalize<seq<reject, T>>
    {
        typedef reject type;
    };

    template<typename Rule>
    struct normalize<optional<Rule>>
    {
        typedef alt<empty, typename normalize<Rule>::type> type;
    };

    template<typename Rule>
    struct normalize<plus<Rule>>
    {
        typedef typename normalize<Rule>::type S;

        typedef seq<S, star<S>> type;
    };

    template<typename Rule>
    struct normalize<star<Rule>>
    {
        typedef typename normalize<Rule>::type S;
        typedef star<S> type;
    };

    template<>
    struct normalize<alt<>>
    {
        typedef reject type;
    };

    template<typename R>
    struct normalize<alt<R>>
    {
        typedef typename normalize<R>::type type;
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
    struct normalize<alt<T,T>>
    {
        typedef typename normalize<T>::type type;
    };

    template<typename T1, typename T2>
    struct normalize<alt<T1,alt<T1,T2>>>
    {
        typedef alt<T1,T2> t;
        typedef typename normalize<t>::type type;
    };

    template<typename T2>
    struct normalize<alt<empty,alt<empty,T2>>>
    {
        typedef alt<empty,T2> t;
        typedef typename normalize<t>::type type;
    };


    template<typename T1, typename T2>
    struct make_nonempty<seq<T1,T2>>
    {
        // Too difficult so just ignore this case

        static const bool empty = false;
        typedef seq<T1,T2> type;

        //typedef make_nonempty<T1> n1;
        //typedef make_nonempty<T2> n2;

        //typedef alt<seq<T1, typename n2::type>, seq<typename n1::type, T2>> type;
        //static const bool empty = n1::empty && n2::empty;
    };

    template<typename T1, typename T2>
    struct make_nonempty<alt<T1,T2>>
    {
        typedef make_nonempty<T1> n1;
        typedef make_nonempty<T2> n2;
        typedef alt<typename n1::type, typename n2::type> t;
        typedef typename normalize<t>::type type;
        static const bool empty = n1::empty || n2::empty;
    };

    template<>
    struct make_nonempty<empty>
    {
        typedef empty type;
        static const bool empty = true;
    };

    template<typename T2>
    struct normalize<alt<empty,T2>>
    {
        typedef typename make_nonempty<T2>::type n2;
        typedef typename normalize<T2>::type t2;
        typedef alt<empty, t2> type;
    };

    template<typename T1, typename T2>
    struct normalize<alt<T1,T2>>
    {
        typedef typename normalize<T1>::type s1;
        typedef typename normalize<T2>::type s2;
        typedef make_nonempty<s1> n1;
        typedef make_nonempty<s2> n2;
        typedef alt<typename n1::type, typename n2::type> t1;

        typedef typename type_if<n1::empty || n2::empty, alt<empty, t1>, t1>::type type;
    };

    template<int Tok, typename R>
    struct normalize<token<Tok, R>>
    {
        typedef token<Tok, typename normalize<R>::type> type;
    };

    template<char...Chs>
    struct normalize<notch<Chs...>>
    {
        typedef notch<Chs...> type;
    };

    template<typename Rule>
    struct normalize<whitespace<Rule>>
    {
        typedef typename normalize<token<Whitespace, Rule>>::type type;
    };
}
