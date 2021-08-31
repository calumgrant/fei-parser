#pragma once

namespace feiparser
{
    template<typename Lexer>
    class LRParser
    {
        Lexer lexer;

        typedef void(*ParseFn)(LRParser&);
        std::vector<ParseFn> states;
    };

    template<typename ParseState>
    void parse()
    {
    }

    template<typename It, typename Rule>
    struct parser
    {

        // Need stacks for stuff...

    };
}