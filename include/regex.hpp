#pragma once

#include "lexer.hpp"

namespace feiparser
{
    template<typename Rule, typename It>
    bool regex_match(It first, It last)
    {
        return lex<Rule>(first, last)!=NoMatch && first == last;
    }

    template<typename Rule, typename It>
    bool regex_search(It first, It last)
    {
        return lex<Rule>(first, last)!=NoMatch && first == last;
    }

    template<typename Rule, int N>
    bool regex_match(const char (&str)[N])
    {
        return regex_match<Rule>(str, str+N-1);
    }
}
