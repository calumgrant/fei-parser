
namespace feiparser
{
static const int Match = -1;
static const int NoMatch = -2;

    template<typename Rule>
    struct accepts;

    template<>
    struct accepts<reject>
    {
        static const bool value = false;
        static const int token = NoMatch;
    };

    template<>
    struct accepts<empty>
    {
        static const bool value = true;
        static const int token = Match;
    };

    template<typename Rule, int Value>
    struct accepts<accept<Rule, Value>>
    {
        static const bool value = can_be_empty<Rule>::value;
        static const int token = Value;
    };

    template<int C1, int C2>
    struct accepts<chrange<C1,C2>>
    {
        static const bool value = false;
        static const int token = NoMatch;
    };

    template<typename T1, typename T2>
    struct accepts<alt<T1,T2>>
    {
        static const bool value = accepts<T1>::value || accepts<T2>::value;
        static const int token = accepts<T1>::value && accepts<T2>::value ? 
            (accepts<T1>::token < accepts<T2>::token ? accepts<T1>::token : accepts<T2>::token) :
            accepts<T1>::value ? accepts<T1>::token : accepts<T2>::token;
    };

    //template<typename R1, typename R2>
    //struct accepts<

    template<typename Rule, int Ch>
    struct next;

    template<int Ch>
    struct next<reject, Ch>
    {
        typedef reject type;
    };

    template<int Ch>
    struct next<empty, Ch>
    {
        typedef reject type;
    };

    template<bool B, typename T, typename F>
    struct type_if;

    template<typename T, typename F>
    struct type_if<true, T, F>
    {
        typedef T type;
    };

    template<typename T, typename F>
    struct type_if<false, T, F>
    {
        typedef F type;
    };

    template<int C1, int C2, int Ch>
    struct next<chrange<C1,C2>, Ch>
    {
        typedef typename type_if<C1<=Ch && Ch<=C2, empty, reject>::type type;
    };

    template<typename T1, typename T2, int Ch>
    struct next<alt<T1,T2>, Ch>
    {
        typedef alt<typename next<T1,Ch>::type, typename next<T2,Ch>::type> t;
        typedef typename simplify<t>::type type;
    };

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
    int lex(It & current, It end)
    {
        using S = typename simplify<Rule>::type;
        
        if(rejects<S>::value) return NoMatch;

        if(accepts<S>::value && current == end)
        {
            return accepts<S>::token;
        }

        switch(*current)
        {
            case 0: return lex0<S, 0>(current, end);
            case 1: return lex0<S, 1>(current, end);

            case 32: return lex0<S, 32>(current, end);

            case 97: return lex0<S, 97>(current, end);
            case 98: return lex0<S, 98>(current, end);
            case 99: return lex0<S, 99>(current, end);
                
        }

        return NoMatch;
    }
}
