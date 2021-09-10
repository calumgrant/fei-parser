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

    char_lexer lexer();
    char_parser parser();

    node member(node object, const char * key);
    bool getValue(node v, std::string & result);
    bool getValue(node v, int & result);
    bool getValue(node v, double & result);
}
