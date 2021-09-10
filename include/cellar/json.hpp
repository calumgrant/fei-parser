#pragma once
#include "parser.hpp"

namespace cellar::json
{
    enum Nodes
    {
        False,
        True,
        Null,
        Object,
        Array,
        Number,
        String,
        Member
    };

    extern char_lexer lexer;
    extern char_parser parser;
}
