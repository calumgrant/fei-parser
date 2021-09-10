
#include <cellar/output.hpp>
#include <cellar/cellar.hpp>
#include <simpletest.hpp>

#include <cassert>
#include <cstring>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <map>

struct foo {};

using namespace cellar;

namespace fp = cellar;

void testAny()
{
    using any = fp::chrange<0,255>;

    assert(!fp::regex_match<any>(""));
    for(auto i=0; i<=255; ++i)
    {
        auto c = (unsigned char)(i);
        assert(fp::regex_match<any>(&c, 1+&c));
    }
}

template<typename Rule>
void check_matches(const char * str)
{
    assert(fp::regex_match<Rule>(str, str+std::strlen(str)));
}

template<typename Rule>
void check_not_matches(const char * str)
{
    assert(!fp::regex_match<Rule>(str, str+std::strlen(str)));
}

void testString()
{
    using e = fp::string<>;
    using a = fp::string<'a'>;
    using abc = fp::string <'a', 'b', 'c'>;

    check_matches<e>("");
    check_not_matches<e>("x");
    
    check_matches<a>("a");
    check_not_matches<a>("b");
    check_not_matches<a>("ab");

    check_matches<abc>("abc");
    check_not_matches<abc>("abcdef");
    check_not_matches<abc>("ab");
}

void testStar()
{
    using as = fp::star<fp::ch<'a'>>;
    
    using ss = fp::star<as>;
    
    check_matches<as>("");
    check_matches<as>("a");
    check_matches<as>("aaaa");
    check_not_matches<as>("b");
    check_not_matches<as>("aab");

    check_matches<ss>("");
    check_matches<ss>("a");
    check_matches<ss>("aaaa");
    check_not_matches<ss>("b");
    check_not_matches<ss>("aab");
}

void testOptional()
{
    using oa = fp::optional<fp::ch<'a'>>;
    
    check_matches<oa>("");
    check_matches<oa>("a");
    check_not_matches<oa>("b");
    check_not_matches<oa>("ab");
}

void testMatch()
{
    using a = fp::ch<'a'>;
    using calum = fp::string<'c', 'a', 'l', 'u', 'm'>;
    using digit = fp::chrange<'0','9'>;
    using lc = fp::chrange<'a','z'>;
    using uc = fp::chrange<'A','Z'>;
    using alpha = fp::alt<lc, uc>;
    using alnum = fp::alt<alpha, digit>;

    using ws = fp::whitespace<fp::plus<fp::chalt<' ', '\r', '\t', '\n'>>>;

    using utf8_continuation = fp::chrange<128, 191>;
    using utf8_2 = fp::seq<fp::chrange<192, 223>, utf8_continuation>;
    using utf8_3 = fp::seq<fp::chrange<224, 239>, utf8_continuation, utf8_continuation>;
    using utf8_4 = fp::seq<fp::chrange<240, 247>, utf8_continuation, utf8_continuation, utf8_continuation>;
    using utf8 = fp::alt<utf8_2, utf8_3, utf8_4>;

    using id = fp::seq<alpha, fp::star<alnum>>;
    
    assert(fp::regex_match<alnum>("a"));
    assert(fp::regex_match<fp::empty>(""));
    assert(!fp::regex_match<alnum>(" "));
    
    check_matches<id>("abc123");
    check_matches<id>("X");
    check_not_matches<id>("");
    check_not_matches<id>("123");
}

void testSearch()
{
    using bcd = fp::string<'b', 'c', 'd'>;

    auto r = fp::regex_search<bcd>("abcdef");
    assert(r.second - r.first == 3);
    assert(fp::regex_match<bcd>(r.first, r.second));
    
    // Find the longest match possible
    
    using as = fp::plus<fp::ch<'a'>>;

    r = fp::regex_search<as>("bbaaaaaabb");
    assert(r.second - r.first == 6);
}

void longTest()
{
    // Tests tail recursion
    // Only do this with tail recursion enabled
#ifdef _NDEBUG
    std::vector as('a', 1000000);
    auto r = fp::regex_match<fp::plus<fp::ch<'a'>>>(as.begin(), as.end());
    if(!r) throw "Failed test";
#endif
}


class StateGraph
{

public:
    template<typename Rule>
    explicit StateGraph(Rule r, bool label) : labelEdges(label)
    {
        using S = typename fp::normalize<Rule>::type;
        std::stringstream ss;
        ss << S();
        initialState = ss.str();
        AddState(S());
    }
    

    std::string initialState;
    std::set<std::string> states;
    std::multimap<std::string, std::string> transitions;
    std::set<std::pair<std::string, std::string>> transitions2;

private:
    bool labelEdges;

    template<typename Rule, int I=0>
    struct AddTransitions
    {
        using next = typename fp::next<Rule, I>::type;
        using S = typename fp::normalize<next>::type;

        static void Add(StateGraph & sg)
        {
            sg.AddTransition(Rule(), S(), I);
            AddTransitions<Rule, I+1>::Add(sg);
        }
    };

    template<typename Rule>
    struct AddTransitions<Rule, 256>
    {
        static void Add(StateGraph & sg) {}
    };

    template<typename Rule>
    void AddState(Rule r)
    {
        if(fp::rejects<Rule>::value) return;

        std::stringstream ss;
        ss << r;
        
        if(states.find(ss.str()) == states.end())
        {
            states.insert(ss.str());
            AddTransitions<Rule>::Add(*this);
        }
    }
    
    void AddState(fp::reject)
    {
        // Do not add the reject state
    }
    
    template<int Ch>
    void AddState(fp::token<Ch, fp::reject>)
    {
        
    }

    template<typename R1>
    void AddTransition(R1 r1, fp::reject r2, int Ch)
    {
        // Do not add the reject transition
    }
    
    template<typename R1, typename R2>
    void AddTransition(R1 r1, R2 r2, int Ch)
    {
        if(fp::rejects<R2>::value) return;
        
        std::stringstream ss1, ss2;
        ss1 << r1;
        if(labelEdges)
        {
            ss2 << "-";
            if(std::isprint(Ch))
                ss2 << '\'' << char(Ch) << '\'';
            else
                ss2 << "'\\" << Ch << '\'';
            ss2 << "-> ";
        }
        ss2 << r2;

        auto p = std::make_pair(ss1.str(), ss2.str());
        
        if(transitions2.find(p) == transitions2.end())
        {
            transitions.insert(p);
            transitions2.insert(p);
            AddState(r2);
        }
    }
};

std::ostream & operator<<(std::ostream & os, const StateGraph & sg)
{
    for(auto & s : sg.states)
    {
        if(s == sg.initialState)
            os << "Initial state ";
        os << s << std::endl;
        
        auto r = sg.transitions.equal_range(s);

        for(auto i=r.first; i!=r.second; ++i)
        {
            os << "    " << i->second << std::endl;
        }
    }
    return os;
}

void viewStateGraph()
{
    using r1 = fp::ch<'a'>;
    using r2 = fp::string<'a', 'b', 'c'>;
    using n2 = fp::normalize<r2>::type;
    using r3 = fp::star<r1>;
    using n3 = fp::next<r3, 'a'>::type;

    std::cout << r1() << std::endl;
    std::cout << r2() << std::endl;
    std::cout << n2() << std::endl;
    std::cout << r3() << std::endl;
    std::cout << n3() << std::endl;

    StateGraph g1 {n3(), true};
    std::cout << g1 << std::endl;

    StateGraph g2 {fp::star<fp::string<'a','b'>>(), true};
    std::cout << g2 << std::endl;

    using r4 = fp::plus<fp::seq<fp::ch<'a'>,fp::optional<fp::ch<'b'>>>>;
    StateGraph g3 {r4(), true};
    std::cout << g3 << std::endl;

    check_matches<r4>("a");
    check_matches<r4>("ab");
    check_matches<r4>("aba");
    check_matches<r4>("abab");
    check_not_matches<r4>("ababb");
}

void testLexer()
{
    using t1 = fp::token<100, fp::ch<'x'>>;
    auto l1 = fp::make_lexer<t1>();
    
    auto t = l1.tokenize("x");
    assert(t.lex());
    assert(!t.lex());

    t = l1.tokenize("ax");
    assert(t.lex());
    assert(!t.lex());

    t = l1.tokenize("aaxaa");
    assert(t.lex());
    assert(!t.lex());
    
    t = l1.tokenize("aaxaaxa");
    assert(t.lex());
    assert(t.lex());
    assert(!t.lex());

    using digit = fp::chrange<'0','9'>;
    using alpha  = fp::alt<fp::chrange<'a','z'>,fp::chrange<'a','z'>>;
    using digits = fp::plus<fp::chrange<'0','9'>>;
    using alnum = fp::alt<alpha, digit>;

    using g2 = fp::alt<
        fp::token<100, digits>,
        fp::token<101, fp::seq<alpha, fp::star<alnum>>>
        >;

    auto l2 = fp::make_lexer<g2>();

    t = l2.tokenize("  123 4 abc g123");
    while(t.lex())
    {
        std::cout << t << std::endl;
    }
}

void testNotCh()
{
    using g1 = fp::notch<'a', 'b'>;

    check_matches<g1>("c");
    check_not_matches<g1>("a");
    check_not_matches<g1>("b");
    check_not_matches<g1>("");
}

void testX()
{
    using namespace cellar;
    using IntegerTypeSuffix = chalt<'l','L'>;
    using NonZeroDigit = chrange<'1','9'>;
    using Digit = alt<ch<'0'>, NonZeroDigit>;
    using DigitOrUnderscore = alt<Digit, ch<'_'>>;
    using Underscores = plus<ch<'_'>>;
    using DigitsAndUnderscores = plus<DigitOrUnderscore>;
    using Digits = alt<Digit, seq<Digit, optional<DigitsAndUnderscores>, Digit>>;
    using DecimalNumeral = alt<ch<'0'>, seq<NonZeroDigit, optional<Digits>>, seq<NonZeroDigit, Underscores, Digits>>;

    using DecimalIntegerLiteralT = seq<DecimalNumeral, optional<IntegerTypeSuffix>>;

    auto g1 =  fp::make_lexer<token<123, DecimalIntegerLiteralT>>();
    // auto g1 =  fp::make_lexer<seq<Underscores, Digits>>();
    // auto g1 =  fp::make_lexer<seq<NonZeroDigit, optional<Digits>>>();
}

void testComment()
{
    using namespace cellar;
    using NotStar = notch<'*'>;
    using NotSlash = notch<'/'>;
    using NotStarNotSlash = notch<'*','/'>;
    using InputCharacter = notch<'\n','\r'>;
    
    using CommentBody = star<alt<NotStar, seq<plus<ch<'*'>>, NotStarNotSlash>>>;
    using TraditionalComment = seq<ch<'/'>, ch<'*'>, CommentBody, plus<ch<'*'>>, ch<'/'>>;
    
    check_matches<TraditionalComment>("/*abc*/");
    check_matches<TraditionalComment>("/**/");
    check_matches<TraditionalComment>("/***/");
    check_matches<TraditionalComment>("/* **/");
    check_matches<TraditionalComment>("/* *** */");
    
    using CommentChar = alt<NotStar, seq<plus<ch<'*'>>, NotStarNotSlash>>;
    check_not_matches<star<ch<'a'>>>("b");
    check_not_matches<CommentChar>("*/");
    check_not_matches<CommentBody>("*/");

    check_not_matches<TraditionalComment>("/**/ ");
    check_not_matches<TraditionalComment>("/*abc*/ ");
    check_not_matches<TraditionalComment>("/* ... */ /* ... */");
    check_not_matches<TraditionalComment>("/***/ ");
    check_not_matches<TraditionalComment>("/* **/ ");
    check_not_matches<TraditionalComment>("/* *** */ ");
}


void testKeywords()
{
    enum Tokens
    {
        Abstract,
        Assert,
        Boolean,
        Break,
        Byte,
        Case,
        Catch,
        Char,
        Class,
        Const,
        Continue,
        Default,
        Do,
        Double,
        Else,
        Enum,
        Extends,
        Final,
        Finally,
        Float,
        For,
        If,
        Goto,
        Implements,
        Import,
        Instanceof,
        Int,
        Interface,
        Long,
        Native,
        New,
        Package,
        Private,
        Protected,
        Public,
        Return,
        Short,
        Static,
        Strictfp,
        Super,
        Switch,
        Synchronized,
        This,
        Throw,
        Transient,
        Try,
        Void,
        Volatile,
        While
    };

    using namespace cellar;

    using KeywordToken = alt<
    /*
        token<Abstract, string<'a', 'b','s','t','r','a','c','t'>>,
        token<Assert, string<'a','s','s','e','r','t'>>,
        token<Boolean, string<'b','o','o','l','e','a','n'>>,
        token<Break, string<'b','r','e','a','k'>>,
        token<Byte, string<'b','y','t','e'>>,
        token<Case, string<'c','a','s','e'>>,
        token<Catch, string<'c','a','t','c','h'>>,
        token<Char, string<'c','h','a','r'>>,
        token<Class, string<'c','l','a','s','s'>>,
        token<Const, string<'c','o','n','s','t'>>,

        token<Continue, string<'c','o','n','t','i','n','u','e'>>,
        token<Default, string<'d','e','f','a','u','l','t'>>,
        token<Do, string<'d','o'>>,
        token<Double, string<'d','o','u','b','l','e'>>,
        token<Else, string<'e','l','s','e'>>,
        token<Enum, string<'e','n','u','m'>>,
        token<Extends, string<'e','x','t','e','n','d','s'>>,
        token<Final, string<'f','i','n','a','l'>>,
        token<Finally, string<'f','i','n','a','l','l','y'>>,
        token<Float, string<'f','l','o','a','t'>>,

        token<For, string<'f','o','r'>>,
        token<If, string<'i','f'>>,
        token<Goto, string<'g','o','t','o'>>,
        token<Implements, string<'i','m','p','l','e','m','e','n','t','s'>>,
        token<Import, string<'i','m','p','o','r','t'>>,
        token<Instanceof, string<'i','n','s','t','a','n','c','e','o','f'>>,
        token<Int, string<'i','n','t'>>,
        token<Interface, string<'i','n','t','e','r','f','a','c','e'>>,
        token<Long, string<'l','o','n','g'>>,
        token<Native, string<'n','a','t','i','v','e'>>,

        token<New, string<'n','e','w'>>,
        token<Package, string<'p','a','c','k','a','g','e'>>,
        token<Private, string<'p','r','i','v','a','t','e'>>,
        token<Protected, string<'p','r','o','t','e','c','t','e','d'>>,
        token<Public, string<'p','u','b','l','i','c'>>,
        token<Return, string<'r','e','t','u','r','n'>>,
        token<Short, string<'s','h','o','r','t'>>,
        token<Static, string<'s','t','a','t','i','c'>>,
        token<Strictfp, string<'s','t','r','i','c','t','f','p'>>,
        token<Super, string<'s','u','p','e','r'>>,

        token<Switch, string<'s','w','i','t','c','h'>>,
        token<Synchronized, string<'s','y','n','c','h','r','o','n','i','z','e','d'>>,
        token<This, string<'t','h','i','s'>>,
        token<Throw, string<'t','h','r','o','w'>>,
        token<Transient, string<'t','r','a','n','s','i','e','n','t'>>,
        token<Try, string<'t','r','y'>>,
        token<Void, string<'v','o','i','d'>>,
    */
        token<Volatile, string<'v','o','l','a','t','i','l'>>,
        token<While, string<'w','h','i','l','e'>>
    >;    

    // StateGraph{KeywordToken(), true};
    //  std::cout << sg;
}

class TestPI : public Test::Fixture<TestPI>
{
public:
    TestPI()
    {

    }


    using S = plus<chalt<' ', '\r', '\n', '\t'>>;

    using NameStartChar = alt<
        chalt<':', '_'>,
        chrange<'A','Z'>,
        chrange<'a','Z'>
        >;

    using NameChar = alt<
        NameStartChar,
        chalt<'-', '.'>,
        ch<0xb7>
        >;

    using Name = seq<NameStartChar, star<NameChar>>;
    
    using NotStar = notch<'?'>;
    using NotStarNotSlash  = notch<'?','>'>;
    using CommentBody = star<alt<NotStar, seq<plus<ch<'?'>>, NotStarNotSlash>>>;
    using PI = seq<ch<'<'>, ch<'?'>, Name, CommentBody, plus<ch<'?'>>, ch<'>'>>;
    using PIToken = token<100, PI>;

    void Test()
    {
        check_matches<PI>("<?xml  ?>");
    }
} pitest;

void testTextBlock()
{
    using namespace cellar;

using UnicodeMarker = plus<ch<'u'>>;
using HexDigit = alt<chrange<'0','9'>,chrange<'a','f'>,chrange<'A','F'>>;
using UnicodeEscape = seq<ch<'\\'>, UnicodeMarker, HexDigit, HexDigit, HexDigit, HexDigit>;

using ZeroToThree = chrange<'0', '3'>;
using OctalDigit = chrange<'0','7'>;

using OctalEscape = alt<
    seq<ch<'\\'>, OctalDigit>,
    seq<ch<'\\'>, OctalDigit, OctalDigit>,
    seq<ch<'\\'>, ZeroToThree, OctalDigit, OctalDigit>
    >;

using EscapeSequence = alt<
    UnicodeEscape, // Standards violation as this should be done by the preprocessor
    string<'\\', 'b'>,
    string<'\\', 't'>,
    string<'\\', 'n'>,
    string<'\\', 'f'>,
    string<'\\', 'r'>,
    string<'\\', '\"'>,
    string<'\\', '\''>,
    string<'\\', '\\'>,
    string<'\\', 's'>,
    OctalEscape
    >;

    using StringCharacter = alt<
        notch<'\r', '\n', '\"', '\\'>,
        EscapeSequence
        >;

    using StringBlockTerminator = string<'\"','\"','\"'>;

    using TextBlockCh = 
        seq<optional<ch<'\"'>>, optional<ch<'\"'>>, notch<'\"'>>;

    // Not in the standard, but a proposal needed to parse the JDK
    using TextBlock = seq<StringBlockTerminator, star<TextBlockCh>, optional<string<'\\','\"'>> ,StringBlockTerminator>;

    using StringLiteral = alt<
        seq<ch<'\"'>, star<StringCharacter>, ch<'\"'>>,
        TextBlock>;

    check_matches<StringLiteral>("\"\"");
    check_matches<StringLiteral>("\"\\\"\"");
    check_matches<StringLiteral>("\"\"\"\"\"\"");
    check_matches<StringLiteral>("\"\"\"\n\"\"\"");
    check_matches<StringLiteral>("\"\"\"  \\\"\"\"\"");
    check_matches<StringLiteral>("\"\"\"  \\\" \"\"\"");
}

int main()
{
    testAny();
    testString();
    testStar();
    testOptional();
    testMatch();
    testSearch();
    longTest();
    testNotCh();
    viewStateGraph();
    testLexer();
    testComment();
    testKeywords();
    testTextBlock();
}
