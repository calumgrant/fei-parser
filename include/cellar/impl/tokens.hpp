#pragma once
#include "normalize.hpp"

namespace cellar
{
    namespace impl
    {
        template<typename Symbol, typename Visited, bool Recursive = tree_contains<Symbol, Visited>::value>
        struct tokens
        {
            using V2 = typename tree_insert<Symbol, Visited>::type;
            using type = typename tokens<typename Symbol::rules, V2>::type;
        };

        template<typename Symbol, typename Visited>
        struct tokens<Symbol, Visited, true>
        {
            using type = empty_tree;
        };

        template<typename Visited>
        struct tokens<symbol<>, Visited, false>
        {
            using type = empty_tree;
        };

        template<int Id, typename...Def, typename Visited>
        struct tokens<token<Id, Def...>, Visited, false>
        {
            using type = token<Id, Def...>;
        };

        template<int Id, typename Visited>
        struct tokens<rule<Id>, Visited, false>
        {
            using type = empty_tree;
        };
    
        template<typename...Ss, typename Visited>
        struct tokens<symbol<Ss...>, Visited, false>
        {
            using type = typename tokens<symbol<Ss...>, Visited>::type;
        };

        template<int Id, typename S, typename...Ss, typename Visited>
        struct tokens<rule<Id, S, Ss...>, Visited, false>
        {
            using T1 = typename tokens<S, Visited>::type;
            using T2 = typename tokens<rule<Id, Ss...>, Visited>::type;
            using type = typename tree_union<T1, T2>::type;
        };

        template<typename S, typename...Ss, typename Visited>
        struct tokens<symbol<S, Ss...>, Visited, false>
        {
            using T1 = typename tokens<S, Visited>::type;
            using T2 = typename tokens<symbol<Ss...>, Visited>::type;
            using type = typename tree_union<T1,T2>::type;
        };

        template<typename List, typename Item>
        struct build_token_list
        {
            using type = alt<Item, List>;
        };

        template<typename Item>
        struct build_token_list<alt<>, Item>
        {
            using type = Item;
        };

        template<typename TokenSet>
        struct make_lexer
        {
            using type = typename forall<TokenSet, alt<>, build_token_list>::type;
        };
    }

    /*
        Constructs a list of all tokens that are mentioned in the grammr.

        This can be used to auto-generate a lexer from a grammar.
    */
    template<typename Symbol>
    struct make_lexer_from_grammar
    {
        using T1 = typename impl::tokens<Symbol, empty_tree>::type;
        using T2 = typename impl::make_lexer<T1>::type;
        using type = typename normalize<T2>::type;
    };
}
