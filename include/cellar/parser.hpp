#pragma once

#include "impl/tree.hpp"

namespace cellar
{
    template<typename It>
    class parser
    {
    public:
        typedef void (*ParseFn)(It, It, tree&);

        parser(ParseFn fn) : fn(fn) {}

        tree parse(It a, It b) const
        {
            tree t;
            parse(a,b,t);
            return t;
        }

        void parse(It a, It b, tree & t) const
        {
            fn(a,b,t);
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
