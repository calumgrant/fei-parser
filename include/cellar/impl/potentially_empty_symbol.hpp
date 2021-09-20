#pragma once

namespace cellar
{
    /*
        Determines whether a grammar symbol can be empty.
        It needs to guard against recursion. 
    */
    template<typename Symbol, typename Visited = empty_tree, bool Recursive = tree_contains<Symbol, Visited>::value>
    struct potentially_empty_symbol
    {
        static const bool value = potentially_empty_symbol<typename Symbol::rules, typename tree_insert<Symbol, Visited>::type>::value;
        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            Symbol,
            Visited,
            tree_contains<Symbol, Visited>, tree_insert<Symbol, Visited>, 
            potentially_empty_symbol<typename Symbol::rules, typename tree_insert<Symbol, Visited>::type>
            >;
    };

    template<typename Symbol, typename Visited>
    struct potentially_empty_symbol<Symbol, Visited, true>
    {
        // Recursive case
        static const bool value = false;
        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            Symbol,
            Visited,
            tree_contains<Symbol, Visited>
            >; 
    };

    template<typename Visited>
    struct potentially_empty_symbol<symbol<>, Visited, false>
    {
        static const bool value = false;

        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            symbol<>,
            Visited,
            tree_contains<symbol<>, Visited>
            >; 
    };

    template<typename Rule, typename...Rules, typename Visited>
    struct potentially_empty_symbol<symbol<Rule, Rules...>, Visited, false>
    {
        static const bool value = potentially_empty_symbol<Rule, Visited>::value || potentially_empty_symbol<symbol<Rules...>, Visited>::value;

        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            symbol<Rule, Rules...>,
            Visited,
            tree_contains<symbol<Rule, Rules...>, Visited>,
            potentially_empty_symbol<Rule, Visited>,
            potentially_empty_symbol<symbol<Rules...>, Visited>            
            >; 

    };

    template<int Id, typename...Definition, typename Visited>
    struct potentially_empty_symbol<token<Id, Definition...>, Visited, false>
    {
        static const bool value = false;

        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            token<Id, Definition...>,
            Visited,
            tree_contains<token<Id, Definition...>, Visited>
            >;

    };

    template<int Id, typename Visited>
    struct potentially_empty_symbol<rule<Id>, Visited, false>
    {
        static const bool value = true;

        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            rule<Id>,
            Visited,
            tree_contains<rule<Id>, Visited>
            >;
    };

    template<int Id, typename Rule, typename...Rules, typename Visited>
    struct potentially_empty_symbol<rule<Id, Rule, Rules...>, Visited, false>
    {
        static const bool value = potentially_empty_symbol<Rule, Visited>::value &&
            potentially_empty_symbol<rule<Id, Rules...>, Visited>::value;

        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            rule<Id>,
            Visited,
            tree_contains<rule<Id, Rule, Rules...>, Visited>,
            potentially_empty_symbol<Rule, Visited>,
            potentially_empty_symbol<rule<Id, Rules...>, Visited>
            >;
    };    
}