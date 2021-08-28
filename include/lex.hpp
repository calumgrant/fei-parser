#pragma once
#include "lexer.hpp"
#include "accepts.hpp"
#include "next.hpp"

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

    template<typename Rule> struct rejects { static const bool value = false; };
    template<> struct rejects<reject> { static const bool value = true; };

    template<typename Rule, typename It>
    void lex1(It & current, It end, int & token, It & matchEnd)
    {
        using S = typename normalize<Rule>::type;

        if(accepts<S>::value)
        {
            token = accepts<S>::token;
            matchEnd = current;
        }

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
        It me;
        int token = NoMatch;
        lex1<Rule>(current, end, token, me);
        if(token != NoMatch)
            current = me; // Backtrack to result
        return token;
    }

    template<typename Rule, typename It=const char*>
    auto make_lexer()
    {
        return lexer<It>(&lex<Rule>);
    }
}
