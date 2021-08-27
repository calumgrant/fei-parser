#include <feiparser.hpp>
#include <cassert>
#include <cstring>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <map>

struct foo {};

namespace fp = feiparser;

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

namespace feiparser
{
    template<int Ch>
    std::ostream & operator<<(std::ostream & os, ch<Ch>)
    {
        return os << char(Ch);
    }

    template<char Ch>
    std::ostream & operator<<(std::ostream & os, string<Ch>)
    {
        return os << char(Ch);
    }

    template<char Ch1, char Ch2, char...Chs>
    std::ostream & operator<<(std::ostream & os, string<Ch1, Ch2, Chs...>)
    {
        return os << char(Ch1) << fp::string<Ch2, Chs...>();
    }

    template<typename R1, typename R2>
    std::ostream & operator<<(std::ostream & os, seq<R1,R2>)
    {
        return os << '(' << R1() << ")(" << R2() << ')';
    }

    template<typename R1, typename R2>
    std::ostream & operator<<(std::ostream & os, alt<R1,R2>)
    {
        return os << '(' << R1() << ")|(" << R2() << ')';
    }

    std::ostream & operator<<(std::ostream & os, empty)
    {
        return os << "()";
    }

    std::ostream & operator<<(std::ostream & os, reject)
    {
        return os << "<reject>";
    }

    template<typename R>
    std::ostream & operator<<(std::ostream & os, star<R>)
    {
        return os << "*(" << R() << ')';
    }
}

class StateGraph
{

public:
    template<typename Rule>
    explicit StateGraph(Rule r)
    {
        using S = typename fp::normalize<Rule>::type;
        AddState(S());
    }

    std::set<std::string> states;
    std::multimap<std::string, std::string> transitions;

private:

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
        std::stringstream ss;
        ss << r;
        
        if(states.find(ss.str()) == states.end())
        {
            states.insert(ss.str());
            AddTransitions<Rule>::Add(*this);
        }
    }

    template<typename R1, typename R2>
    void AddTransition(R1 r1, R2 r2, int Ch)
    {
        std::stringstream ss1, ss2;
        ss1 << r1;
        ss2 << r2;
        transitions.insert(std::make_pair(ss1.str(), ss2.str()));
        AddState(r2);
    }
};

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

    StateGraph g1 {n3()};

    StateGraph g2 {fp::star<fp::string<'a','b'>>()};
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

    viewStateGraph();
}
