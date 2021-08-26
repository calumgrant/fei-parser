#pragma once

/*
    Defines simplify<>, so rewrite rules into a more simple form.
    This also desugars more complex rules into simpler elements.
*/

namespace feiparser
{
    // Rewrites lexer rules to a canonical form.
    template<typename Rule>
    struct simplify;

    // The base types do not simplify
    template<int Ch>
    struct simplify<ch<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<>
    struct simplify<chseq<>>
    {
        typedef empty type;
    };

    template<int Ch>
    struct simplify<chseq<Ch>>
    {
        typedef ch<Ch> type;
    };

    template<int Ch, int...Chs>
    struct simplify<chseq<Ch, Chs...>>
    {
        typedef seq<ch<Ch>, typename simplify<chseq<Chs...>>::type> type;
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

    template<typename T1, typename T2>
    struct simplify<alt<T1,T2>>
    {
        // NB: Could simplify further here
        typedef alt<typename simplify<T1>::type, typename simplify<T2>::type> type;
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

    template<typename...Rules>
    struct simplify<seq<empty, Rules...>>
    {
        typedef typename simplify<seq<Rules...>>::type type;
    };

    template<>
    struct simplify<reject>
    {
        typedef reject type;
    };

    template<typename...Rules>
    struct simplify<seq<reject, Rules...>>
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

    template<typename...Rules>
    struct simplify<alt<Rules...>>
    {
        // Algorithm: Implement lift_empty_rule on all members

    };

    template<typename Rule>
    struct lift_empty_rules;

    template<typename H, typename...T>
    struct lift_empty_rules<alt<H, T...>>
    {
        typedef typename lift_empty_rules<alt<T...>>::type tail;
        static const bool is_empty = tail::is_empty;
    };

    template<typename...Rules>
    struct lift_empty_rules<alt<empty, Rules...>>
    {
        static const bool is_empty = true;
    };

    template<typename...Rules>
    struct can_be_empty;

    template<typename... Rules>
    struct can_be_empty<empty, Rules...>
    {
        static const bool value = true;
    };

}
