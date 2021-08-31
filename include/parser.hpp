#pragma once

#include "tree.hpp"

namespace feiparser
{
    template<typename It>
    class parser
    {
    public:
        typedef void (*ParseFn)(It start, It end, tree & output);

        parser(ParseFn fn) : fn(fn) {}

        void parse(It start, It end, tree & output) const
        {
            fn(start, end, output);
        }

        tree parse(It start, It end) const
        {
            tree tree;
            parse(start, end, tree);
            return std::move(tree);
        }

    private:
        ParseFn fn;
    };

    typedef parser<const char*> char_parser;
}
