#include <cellar/cellar.hpp>

#include <simpletest.hpp>

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

// Create a large set

// Are typelists more efficient??
struct empty_typelist {};

template<typename H, typename T>
struct typelist {};

template<typename Item, typename Tl>
struct typelist_contains;

template<typename Item>
struct typelist_contains<Item, empty_typelist>
{
    static const bool value = false;
};

template<typename Item, typename T>
struct typelist_contains<Item, typelist<Item, T>>
{
    static const bool value = true;
};

template<typename Item, typename H, typename T>
struct typelist_contains<Item, typelist<H, T>>
{
    static const bool value = typelist_contains<Item, T>::value;
};

template<typename Item, typename Tl, bool Contains = typelist_contains<Item, Tl>::value>
struct typelist_insert
{
    using type = Tl;
};

template<typename Item, typename Tl>
struct typelist_insert<Item, Tl, false>
{
    using type = typelist<Item, Tl>;
};

template<typename Tl>
struct typelist_size;

template<>
struct typelist_size<empty_typelist>
{
    static const int value = 0;
};

template<typename H, typename T>
struct typelist_size<typelist<H,T>>
{
    static const int value = 1 + typelist_size<T>::value;
};




template<typename Item, typename... Items>
struct typeset_contains2a;

// Size = 0
template<typename I>
struct typeset_contains2a<I>
{
    static const bool value = false;
};

// Size = 1

template<typename I, typename I0>
struct typeset_contains2a<I, I0>
{
    static const bool value = false;
};

template<typename I>
struct typeset_contains2a<I, I>
{
    static const bool value = true;
};

// Size >= 2

template<typename I, typename I0, typename I1, typename...Is>
struct typeset_contains2a<I, I0, I1, Is...>
{
    static const bool value = typeset_contains2a<I, Is...>::value;
};

template<typename I, typename I1, typename...Is>
struct typeset_contains2a<I, I, I1, Is...>
{
    static const bool value = true;
};

template<typename I, typename I0, typename...Is>
struct typeset_contains2a<I, I0, I, Is...>
{
    static const bool value = true;
};

template<typename Item, typename Ts>
struct typeset_contains2;

template<typename Item, typename... Items>
struct typeset_contains2<Item, typeset<Items...>>
{
    static const bool value = typeset_contains2a<Item, Items...>::value;
};

template<typename Item, typename Ts, bool contains = typeset_contains2<Item, Ts>::value>
struct typeset_insert2
{
    using type = Ts;
};

template<typename Item, typename...Items>
struct typeset_insert2<Item, typeset<Items...>, false>
{
    using type = typeset<Item, Items...>;
};

template<int N>
struct build_typelist
{
    using P = typename build_typelist<N-1>::type;
    using type = typename typelist_insert<token<N>, P>::type;

};

template<int N>
struct build_typeset
{
    using P = typename build_typeset<N-1>::type;
    using type = typename typeset_insert2<token<N>, P>::type;
};

template<>
struct build_typeset<0>
{
    using type = typeset<>;
    using type2 = empty_typelist;
};

template<>
struct build_typelist<0>
{
    using type = empty_typelist;
};

template<int N>
struct testcase
{
    using S = typename build_typeset<N>::type;
    using profile_tag = no_tag;
    using profile_types = profile_types<S>;
};

// using test1 = testcase<301>;

template<int N>
struct tl_testcase
{
    using S = typename build_typelist<N>::type;
    // static const int value = typelist_size<S>::value;
};

using test2 = tl_testcase<1000>;

int main()
{
    test2::S();
    // std::cout << "typelist size = " << test2::value << std::endl;
    //profile_template<test1>();
    //std::cout << "Number of typeset<> = " << static_count<typeset_tag>() << std::endl;
}
