#pragma once

namespace cellar
{
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
}