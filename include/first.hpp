#pragma once

namespace feiparser
{
   /*
        Constructs a typeset of the "first" terminal/token symbol in a symbol.
        It needs to guard against recursion.
    */
    template<typename Symbol, typename Visited = typeset<>, bool Recursive = typeset_contains<Symbol, Visited>::value>
    struct first
    {
        // Base case is to look at the "rules" member of a user-defined type.
        using type = typename first<typename Symbol::rules, typename typeset_insert<Symbol, Visited>::type>::type;
    };

    template<typename Symbol, typename Visited>
    struct first<Symbol, Visited, true>
    {
        using type = typeset<>;
    };

    template<typename Visited>
    struct first<symbol<>, Visited, false>
    {
        using type = typeset<>;
    };

    template<typename Rule, typename... Rules, typename Visited>
    struct first<symbol<Rule, Rules...>, Visited, false>
    {
        using S1 = typename first<Rule, Visited>::type;
        using S2 = typename first<symbol<Rules...>, Visited>::type;
        using type = typename typeset_union<S1,S2>::type;
    };

    template<int Id, typename Visited>
    struct first<rule<Id>, Visited, false>
    {
        using type = typeset<>;        
    };

    template<int Id, typename...Def, typename Visited>
    struct first<token<Id, Def...>, Visited, false>
    {
        using type = typeset<token<Id>>;
    };

    template<int Id, typename Symbol, typename...Symbols, typename Visited>
    struct first<rule<Id, Symbol, Symbols...>, Visited, false>
    {
        using S1 = typename first<Symbol, Visited>::type;
        using S2 = typename first<rule<Id, Symbols...>, Visited>::type;
        using U = typename typeset_union<S1,S2>::type;
        using type = typename type_if<potentially_empty_symbol<Symbol>::value, U, S1>::type;
    };
}
