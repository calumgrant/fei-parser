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

    template<typename T1, typename T2>
    struct type_equals
    {
        static const bool value = false;
    };

    template<typename T>
    struct type_equals<T,T>
    {
        static const bool value = true;
    };
}
