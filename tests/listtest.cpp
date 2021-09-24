#include <simpletest.hpp>
#include "typeset_test.hpp"


using T1 = make_random<10, 1, 6>::type;
using T2 = make_range<0, 10>::type;

static_assert(size<T1>::value==10, "");
static_assert(size<T2>::value==10, "");

using T3 = make_list<>::type;

static_assert(size<T3>::value==0, "");

using T4 = make_list<token<1>, token<0>>::type;
static_assert(size<T4>::value == 2);
using T4 = make_list<token<0>, token<1>>::type;

//using l0 = make_list<>::type;
//using l1 = make_list<>::type;

template<int Item, typename List>
struct build_list
{
    using type = typename insert<token<Item>, List>::type;
};


template<typename List, int N>
struct list_test
{
    using T5 = typename loop<make_range<0, 100>::type, List, build_list>::type;
    using T6 = typename loop<make_random<500, 0, 1000>::type, List, build_list>::type;

    using T7 = typename merge<T5, T6>::type;

    static_assert(contains<token<20>, T5>::value, "");
    static_assert(contains<token<20>, T7>::value, "");
    static_assert(!contains<token<-1>, T7>::value, "");
    static_assert(!contains<token<-1>, List>::value, "");

    static void output()
    {
        write<T5>(std::cout);
        std::cout << std::endl;
        write<T6>(std::cout);
        std::cout << std::endl;
        write<T7>(std::cout);
        std::cout << std::endl;
    }
};

using Test1 = list_test<make_list<>::type, 500>;


int main()
{
    Test1::output();
}
