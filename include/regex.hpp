#pragma once

#include "lexer.hpp"

namespace feiparser
{
    template<typename Rule, typename It>
    bool regex_match(It first, It last)
    {
        int tok;
        It accept_position;
        return lex<Rule>(first, last)!=NoMatch && first == last;
    }

    template<typename Rule, int N>
    bool regex_match(const char (&str)[N])
    {
        return regex_match<Rule>(str, str+N-1);
    }
}
