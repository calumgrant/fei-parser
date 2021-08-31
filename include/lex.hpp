#pragma once
#include "lexer.hpp"
#include "accepts.hpp"
#include "next.hpp"

#if VERBOSE_LEXER
#include <iostream>
#include "output.hpp"
#endif


namespace feiparser
{

    template<typename Rule, typename It>
    void lex1(It & current, It end, int & token, It & matchEnd);

    template<typename Rule, int Ch, typename It>
    void lex0(It & current, It end, int & token, It & matchEnd)
    {
        ++current;
        typedef typename normalize<Rule>::type S;
        typedef typename next<S,Ch>::type N;
        typedef typename normalize<N>::type SN;
        return lex1<SN>(current, end, token, matchEnd);
    }

    // Move to a new file !!
    template<typename Rule> struct rejects { static const bool value = false; };
    template<> struct rejects<reject> { static const bool value = true; };
    template<int Tok> struct rejects<token<Tok, reject>> { static const bool value = true; };

    template<typename T1, typename T2>
    struct rejects<alt<T1,T2>>
    {
        static const bool value = rejects<T1>::value && rejects<T2>::value;
    };

    template<typename Rule, typename It>
    void lex1(It & current, It end, int & token, It & matchEnd)
    {
        using S = typename normalize<Rule>::type;
        
        if(accepts<S>::value)
        {
            token = accepts<S>::token;
            matchEnd = current;
        }

#if VERBOSE_LEXER
        if(accepts<S>::value)
            std::cout << "accepts - ";
        std::cout << Rule() << std::endl;
#endif

        if(rejects<S>::value || current == end)
        {
            return;
        }

        switch((unsigned char)*current)
        {
#define FP_CASE(N) case N: return lex0<S, N>(current, end, token, matchEnd);
#define FP_CASE_BLOCK(N) FP_CASE(N) FP_CASE(N+1) FP_CASE(N+2) FP_CASE(N+3) FP_CASE(N+4) FP_CASE(N+5) FP_CASE(N+6) FP_CASE(N+7) FP_CASE(N+8) FP_CASE(N+9)
                
                FP_CASE_BLOCK(0)
                FP_CASE_BLOCK(10)
                FP_CASE_BLOCK(20)
                FP_CASE_BLOCK(30)
                FP_CASE_BLOCK(40)
                FP_CASE_BLOCK(50)
                FP_CASE_BLOCK(60)
                FP_CASE_BLOCK(70)
                FP_CASE_BLOCK(80)
                FP_CASE_BLOCK(90)
                FP_CASE_BLOCK(100)
                FP_CASE_BLOCK(110)
                FP_CASE_BLOCK(120)
                FP_CASE_BLOCK(130)
                FP_CASE_BLOCK(140)
                FP_CASE_BLOCK(150)
                FP_CASE_BLOCK(160)
                FP_CASE_BLOCK(170)
                FP_CASE_BLOCK(180)
                FP_CASE_BLOCK(190)
                FP_CASE_BLOCK(200)
                FP_CASE_BLOCK(210)
                FP_CASE_BLOCK(220)
                FP_CASE_BLOCK(230)
                FP_CASE_BLOCK(240)
                FP_CASE(250)
                FP_CASE(251)
                FP_CASE(252)
                FP_CASE(253)
                FP_CASE(254)
                FP_CASE(255)
        }
    }

    template<typename Rule, typename It>
    int lex(It & current, It end)
    {
        if(current == end) return EndOfStream;

        It me = end;
        int token = NoMatch;
        using S = typename normalize<Rule>::type;
        lex1<S>(current, end, token, me);
        if(token != NoMatch)
            current = me; // Backtrack to result
        return token;
    }

    template<typename Rule>
    struct is_valid_lexer
    {
        static const bool value = false;
        static_assert(value, "Lexer is invalid here");
    };

    template<int Ch, typename Rule>
    struct is_valid_lexer<token<Ch, Rule>>
    {
        static const bool value = true;
    };

    template<typename Rule1, typename Rule2>
    struct is_valid_lexer<alt<Rule1, Rule2>>
    {
        static const bool value = is_valid_lexer<Rule1>::value && is_valid_lexer<Rule2>::value;
    };

    template<typename Rule, typename It=linecounter<const char*>>
    auto make_lexer()
    {
        using S = typename normalize<Rule>::type;
        static_assert(!accepts<S>::value, "Lexers cannot accept the empty token");
        static_assert(is_valid_lexer<S>::value, "Lexer rule is invalid - must consist of only token<> and alt<>");
        return lexer<It>(&lex<S>);
    }
}
