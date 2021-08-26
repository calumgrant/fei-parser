#include <feiparser.hpp>
#include <cassert>

struct foo {};

namespace fp = feiparser;

int main()
{
    using a = fp::ch<'a'>;
    using calum = fp::chseq<'c', 'a', 'l', 'u', 'm'>;
    using digit = fp::chrange<'0','9'>;
    using lc = fp::chrange<'a','z'>;
    using uc = fp::chrange<'A','Z'>;
    using alpha = fp::alt<lc, uc>;
    using alnum = fp::alt<alpha, digit>;

    using ws = fp::whitespace<fp::plus<fp::chalt<' ', '\r', '\t', '\n'>>>;

    assert(fp::regex_match<alnum>("a"));
    assert(!fp::regex_match<alnum>(" "));
}
