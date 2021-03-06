/*
    lexer.hpp
    Classes needed to tokenize a stream/chunk of text.
*/

#pragma once
#include <utility>
#include "impl/linecounter.hpp"
#include "impl/node_types.hpp"

namespace cellar
{
    template<typename It>
    class token_stream
    {
    public:
        typedef int(*LexerFn)(It & current, It end);

        token_stream(LexerFn fn, It start, It end) : fn(fn), tok_start(start), tok_end(start), stream_end(end)
        {
        }

        bool lex()
        {
            do
            {
                tok_start = tok_end;
                token_value = fn(tok_end, stream_end);
                if(tok_start == tok_end)
                {
                    token_value = EndOfStream;
                    return false;
                }
            }
            while(token_value<0);
            return true;
        }

        bool lex_including_whitespace()
        {
            tok_start = tok_end;
            token_value = fn(tok_end, stream_end);
            return tok_start != tok_end;
        }

        typedef It iterator;
        typedef int token_type;
        typedef std::size_t size_type;

        iterator begin() const { return tok_start; }
        iterator end() const { return tok_end; }
        int token() const { return token_value; }
        size_type size() const { return end() - begin(); }

    private:
        LexerFn fn;
        int token_value;
        It tok_start, tok_end, stream_end;
    };

    template<typename It>
    class lexer
    {
    public:
        typedef int(*LexerFn)(It & current, It end);

        lexer(LexerFn fn) : fn(fn) {}

        bool regex_match(It start, It end) const
        {
            return fn(start, end)!=NoMatch && start==end;
        }

        template<int N>
        bool regex_match(const char (&str)[N]) const
        {
            return regex_match(str, str+N-1);
        }

        std::pair<It,It> regex_search(It start, It end) const
        {
        }

        token_stream<It> tokenize(It a, It b) const
        {
            return token_stream<It>(fn, a, b);
        }

        template<int N>
        auto tokenize(const char (&str)[N])
        {
            return tokenize(str, str+N-1);
        }
        
    private:
        LexerFn fn;
    };

    typedef lexer<linecounter<const char*>> char_lexer;
}
