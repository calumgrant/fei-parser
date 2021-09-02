#pragma once

#include "tree.hpp"

namespace feiparser
{
    template<typename It>
    class parser
    {
    public:
        typedef tree (*ParseFn)(It, It);

        parser(ParseFn fn) : fn(fn) {}

        tree parse(It a, It b)
        {

        }

    private:
        ParseFn fn;
    };

    typedef parser<const char*> char_parser;
}
