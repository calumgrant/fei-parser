/*
    Contains various utilities to help diagnose 
*/

#pragma once
#include "cellar.hpp"

namespace cellar
{
    template<typename State, typename Gathered = typeset<>, bool Exists = typeset_contains<State, Gathered>>
    struct gather_states;

    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, true>
    {
        using type = Gathered;
    };

    template<typename State, typename Gathered>
    struct gather_states<State, Gathered, true>
    {
        using T0 = typename typeset_insert<State, Gathered>::type;

        // Get all of the shifts and the gotos

        
    };

}
