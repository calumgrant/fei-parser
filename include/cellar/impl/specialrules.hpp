/*
    This file contains some common grammatical constructs to extend
    the basic grammar rules.
*/

#pragma once

namespace cellar
{
    // Defines a list of nodes, similar to writing out
    // X -> T
    // X -> X T
    template<typename T, int Id=Hidden>
    class List : public symbol<T, rule<Id, List<T>, T>> {};

    // Defines a sequences of nodes, separated by a token
    // X -> T
    // X -> X sep T
    template<typename T, typename Separator, int Id=Hidden>
    class Sequence : public symbol<T, rule<Id, Sequence<T, Separator, Id>, Separator, T>> {};

    // Defines an optional node, similar to writing out
    // X -> 
    // X -> T
    template<int Id, typename T>
    class Optional : public symbol<rule<Id>, rule<Id, T>> {};

    // Defines an optional list
    template<int Id, typename T>
    class OptionalList : public Optional<Id, List<T>> {};

    // Defines a token that is hidden in the parse tree
    // by giving it the "Removed" node-id.
    template<int Id>
    class Tok : public symbol<rule<Removed, token<Id>>> {};
}
