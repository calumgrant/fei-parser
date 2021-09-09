/*
    This contains declarations to USE a parser.
    If you need to construct a parser (ideally in a separate file),
    you need to include "make_parser.hpp" instead.
*/

#pragma once

#include "impl/tree.hpp"
#include "impl/node_types.hpp"
#include "lexer.hpp"

namespace cellar
{
    template<typename It>
    class parser
    {
    public:
        typedef void (*ParseFn)(const token_stream<linecounter<It>> &, tree&);

        parser(lexer<linecounter<It>> lex, ParseFn fn) : lex(lex), fn(fn) {}

        tree parse(It a, It b) const
        {
            tree t;
            parse(a,b,t);
            return t;
        }

        void parse(It a, It b, tree & t) const
        {
            auto tokens = lex.tokenize(a,b);
            fn(tokens,t);
        }

        template<int N>
        tree parse(const char(&str)[N]) const
        {
            return parse(str, str+N-1);
        }

        tree parseFile(const char * filename) const;

    private:
        lexer<linecounter<It>> lex;
        ParseFn fn;
    };

    typedef parser<const char*> char_parser;
}
