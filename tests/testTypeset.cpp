#include <cellar/cellar.hpp>

using namespace cellar;

template<int X> struct Int {};

using E = typeset<>;
using E1 = typeset<Int<1>>;
using E1 = typename typeset_insert<Int<1>, E>::type;
using E1 = typename typeset_insert<Int<1>, E1>::type;
using E12 = typename typeset_insert<Int<2>, E1>::type;
using E12 = typename typeset_insert<Int<1>, E12>::type;
using E12 = typename typeset_insert<Int<2>, E12>::type;

using E12 = typename typeset_union<E12, E12>::type;
using E12 = typename typeset_union<E1, E12>::type;

using R1 = rule<100>;
using R2 = rule<101, token<1>, token<2>>;
struct S;
using C1 = typeset<rule_position<S, R2, 0, 0>>;
using C2 = typename closure<C1>::type;

int main()
{

}
