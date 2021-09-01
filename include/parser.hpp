#pragma once

#include "lexer.hpp"
#include "tree.hpp"

namespace feiparser
{

    // Should really be hidden/moved from here
    template<typename It>
    struct parse_state
    {
        typedef void(*NextFn)(int id, parse_state&);
        std::vector<NextFn> stack;
        tree parse_tree;
        token_stream<It> tokens;
    };


    template<typename It>
    class parser
    {
    public:
        typedef void (*ParseFn)(parse_state<It>&);

        parser(const lexer<It> & lexer, ParseFn fn) : lexer(lexer), fn(fn) {}

        tree parse(It start, It end) const
        {
            parse_state<It> state(lexer.tokenize(start, end));
            fn(state);
            return std::move(state.tree);
        }

    private:
        lexer<It> lexer;
        ParseFn fn;
    };

    template<typename Language, typename It = const char*>
    class parser2
    {
        // ??
    };

    typedef parser<const char*> char_parser;


    template<typename Rule, typename It>
    void parsefn(parse_state<It> &)
    {

    }

    template<typename Rule, int position>
    struct rule_position {};

    template<typename Rule, typename It>
    auto make_parser(const lexer<It> & lex)
    {
        //typedef typeset<rule_position<rule<0, Rule, token<EndOfStream>>> InitialState;
        using InitialState = Rule;
        return parser<It>(lex, &parsefn<InitialState>);
    }
}
