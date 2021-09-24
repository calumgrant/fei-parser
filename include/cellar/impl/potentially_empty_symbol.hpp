#pragma once


namespace cellar
{
    namespace impl
    {
        template<typename Symbol, int Iteration>
        struct maybe_empty2;

        template<typename Symbol, int Iteration>
        struct maybe_empty
        {
            static const bool value = maybe_empty2<typename Symbol::rules, Iteration>::value;
        };

        template<typename Symbol>
        struct maybe_empty<Symbol, 0>
        {
            static const bool value = false;
        };

        template<int I>
        struct maybe_empty2<symbol<>, I>
        {
            static const bool value = false;
        };

        template<typename Rule, typename...Rules, int I>
        struct maybe_empty2<symbol<Rule, Rules...>, I>
        {
            static const bool value = maybe_empty<typename Rule::rules, I-1>::value || maybe_empty2<symbol<Rules...>, I>::value;
        };

        template<int Id, typename...Def, int I>
        struct maybe_empty2<token<Id, Def...>, I>
        {
            static const bool value = false;
        };

        template<int Id, int I>
        struct maybe_empty2<rule<Id>, I>
        {
            static const bool value = true;
        };

        template<int I, bool Empty, typename... Symbols>
        struct maybe_empty_rule;

        template<int I, typename Symbol, typename...Symbols>
        struct maybe_empty_rule<I, true, Symbol, Symbols...>
        {
            static const bool value = maybe_empty_rule<I, maybe_empty<Symbol, I-1>::value, Symbols...>::value;
        };

        template<int I, typename... Symbols>
        struct maybe_empty_rule<I, false, Symbols...>
        {
            static const bool value = false;
        };

        template<int I>
        struct maybe_empty_rule<I, true>
        {
            static const bool value = true;
        };

        template<int Id, typename S, typename...Ss, int I>
        struct maybe_empty2<rule<Id, S, Ss...>, I>
        {
            static const bool value = maybe_empty_rule<I, maybe_empty<S, I-1>::value, Ss...>::value;
        };
    }

    template<typename Symbol>
    struct potentially_empty_symbol
    {
        // TODO: Tweak this number
        static const bool value = impl::maybe_empty<Symbol, 12>::value;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };
}


namespace cellar_deleteme
{
    using namespace cellar;
    /*
        Determines whether a grammar symbol can be empty.
        It needs to guard against recursion. 
    */
    template<typename Symbol, typename Visited = empty_tree, bool Recursive = contains<Symbol, Visited>::value>
    struct potentially_empty_symbol
    {
        static const bool value = potentially_empty_symbol<typename Symbol::rules, typename insert<Symbol, Visited>::type>::value;
        using profile_tag = potentially_empty_symbol_tag;
        using profile_types = profile<
            Symbol,
            Visited,
            contains<Symbol, Visited>, insert<Symbol, Visited>, 
            potentially_empty_symbol<typename Symbol::rules, typename insert<Symbol, Visited>::type>
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
            contains<Symbol, Visited>
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
            contains<symbol<>, Visited>
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
            contains<symbol<Rule, Rules...>, Visited>,
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
            contains<token<Id, Definition...>, Visited>
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
            contains<rule<Id>, Visited>
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
            contains<rule<Id, Rule, Rules...>, Visited>,
            potentially_empty_symbol<Rule, Visited>,
            potentially_empty_symbol<rule<Id, Rules...>, Visited>
            >;
    };    
}