#include <feiparser.hpp>
#include <cassert>

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

int main()
{
    testAny();
    using a = fp::ch<'a'>;
    using calum = fp::chseq<'c', 'a', 'l', 'u', 'm'>;
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

    assert(fp::regex_match<alnum>("a"));
    assert(fp::regex_match<fp::empty>(""));
    assert(!fp::regex_match<alnum>(" "));
}
