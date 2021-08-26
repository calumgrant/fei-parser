#include <feiparser.hpp>
#include <cassert>
#include <cstring>

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

int main()
{
    testAny();
    testString();
    testStar();
    testOptional();
    testMatch();
}
