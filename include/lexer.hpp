#pragma once
#include <utility>

namespace feiparser
{
    static const int Match = -1;
    static const int NoMatch = -2;
    static const int Whitespace = -3;

    template<typename It>
    class lexer
    {
    public:
        typedef int(*lex)(It & current, It end);

        lexer(lex fn) : fn(fn) {}

        bool regex_match(It start, It end)
        {
            return fn(start, end)!=NoMatch && start==end;
        }

        template<int N>
        bool regex_match(const char (&str)[N])
        {
            return regex_match(str, str+N-1);
        }

        std::pair<It,It> regex_search(It start, It end)
        {

        }
    private:
        lex fn;
    };

    typedef lexer<const char*> char_lexer;
}
