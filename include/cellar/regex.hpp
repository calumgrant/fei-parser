#pragma once

#include <utility>
#include "impl/lex.hpp"

namespace cellar
{
    template<typename Rule, typename It>
    bool regex_match(It first, It last)
    {
        return lex<Rule>(first, last)!=NoMatch && first == last;
    }

    template<typename Rule, typename It>
    std::pair<It,It> regex_search(It first, It last)
    {
        while(first != last)
        {
            auto start = first;
            if(lex<Rule>(first,last)!=NoMatch)
            {
                return std::make_pair(start, first);
            }
        }
        return std::make_pair(last,last);
    }

    template<typename Rule, int N>
    bool regex_match(const char (&str)[N])
    {
        return regex_match<Rule>(str, str+N-1);
    }

    template<typename Rule, int N>
    std::pair<const char*, const char*> regex_search(const char (&str)[N])
    {
        return regex_search<Rule>(str, str+N-1);
    }

}
