#pragma once
#include "accepts.hpp"
#include "next.hpp"

namespace feiparser
{
    template<typename Rule, typename It>
    int lex(It & current, It end);

    template<typename Rule, int Ch, typename It>
    int lex0(It & current, It end)
    {
        ++current;
        typedef typename next<Rule,Ch>::type n;
        typedef typename simplify<n>::type s;
        return lex<s>(current, end);
    }

    template<typename Rule> struct rejects { static const bool value = false; };
    template<> struct rejects<reject> { static const bool value = true; };

    template<typename Rule, typename It>
    int lex1(It & current, It end)
    {
        using S = typename simplify<Rule>::type;

        if(rejects<S>::value || current == end)
        {
            return NoMatch;
        }

        switch((unsigned char)*current)
        {
#define FP_CASE(N) case N: return lex0<S, N>(current, end);
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

        return NoMatch;
    }

    template<typename Rule, typename It>
    int lex(It & current, It end)
    {
        using S = typename simplify<Rule>::type;

        auto c0 = current;
        auto tok = lex1<S>(current, end);
        
        if(tok == NoMatch && accepts<S>::value)
        {
            current = c0;
            return accepts<S>::token;
        }
        
        return tok;
    }
}
