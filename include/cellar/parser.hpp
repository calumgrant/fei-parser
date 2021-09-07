#pragma once

#include "impl/tree.hpp"

namespace cellar
{
    template<typename It>
    class parser
    {
    public:
        typedef tree (*ParseFn)(It, It);

        parser(ParseFn fn) : fn(fn) {}

        tree parse(It a, It b) const
        {
            return fn(a,b);
        }

        template<int N>
        tree parse(const char(&str)[N]) const
        {
            return parse(str, str+N-1);
        }

        tree parseFile(const char * filename) const;

    private:
        ParseFn fn;
    };

    typedef parser<const char*> char_parser;
}
