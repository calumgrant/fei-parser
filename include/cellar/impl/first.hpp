#pragma once
#include "utils.hpp"

namespace cellar
{
    namespace impl
    {
        template<typename Symbol, int Iteration>
        struct first;

        /*
            Constructs a typeset of the "first" terminal/token symbol in a symbol.
            It needs to guard against recursion.
        */
        template<typename Symbol, int Iteration>
        struct first2
        {
            // Base case is to look at the "rules" member of a user-defined type.
            using type = typename first2<typename Symbol::rules, Iteration>::type;

            using profile_tag = first_tag;
            using profile_types = profile<>;
        };

        template<int I>
        struct first2<symbol<>, I>
        {
            using type = make_list<>::type;

            using profile_tag = first_tag;
            using profile_types = profile<symbol<>>;
        };

        template<typename Rule, typename... Rules, int I>
        struct first2<symbol<Rule, Rules...>, I>
        {
            using S1 = typename first<Rule, I-1>::type;
            using S2 = typename first<symbol<Rules...>, I>::type;
            using type = typename merge<S1,S2>::type;

            using profile_tag = first_tag;
            using profile_types = profile<
                >;
        };

        template<int Id, int I>
        struct first2<rule<Id>, I>
        {
            using type = make_list<>::type;    
            
            using profile_tag = first_tag;
            using profile_types = profile<
                >;
        };

        template<int Id, typename...Def, int I>
        struct first2<token<Id, Def...>, I>
        {
            using type = typename make_list<token<Id>>::type;

            using profile_tag = first_tag;
            using profile_types = profile<
                >;
        };

        template<int Id, typename Symbol, typename...Symbols, int I>
        struct first2<rule<Id, Symbol, Symbols...>, I>
        {
            using S1 = typename first<Symbol, I-1>::type;
            using S2 = typename first<rule<Id, Symbols...>, I>::type;
            using U = typename merge<S1,S2>::type;
            using type = typename type_if<potentially_empty_symbol<Symbol>::value, U, S1>::type;

            using profile_tag = first_tag;
            using profile_types = profile<
                >;
        };

        template<typename Symbol, int Iteration>
        struct first
        {
            using type = typename first2<Symbol, Iteration>::type;
        };

        template<typename Symbol>
        struct first<Symbol, 0>
        {
            using type = make_list<>::type;
        };
    }

    template<typename Symbol>
    struct first
    {
        using type = typename impl::first<Symbol, 15>::type;
        using profile_tag = first_tag;
        using profile_types = profile<>; // TODO
    };
}
