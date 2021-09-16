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
struct typelist
{
    using head = H;
    using tail = T;
};

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

// Sorted_insert

template<typename Item, typename Tl>
struct sorted_insert;

template<typename Item>
struct sorted_insert<Item, empty_typelist>
{
    using type = typelist<Item, empty_typelist>;
};

template<typename Item, typename T>
struct sorted_insert<Item, typelist<Item, T>>
{
    using type = typelist<Item, T>;
};

template<typename Item, typename H, typename T, bool Less = compare<Item, H>::less>
struct sorted_insert2
{
    // Linear step
    using type = typelist<H, typename sorted_insert<Item, T>::type>;
};

template<typename Item, typename H, typename T>
struct sorted_insert2<Item, H, T, true>
{
    using type = typelist<Item, typelist<H,T>>;
};

template<typename Item, typename H, typename T>
struct sorted_insert<Item, typelist<H, T>>
{
    using type = typename sorted_insert2<Item, H,T>::type;
};




// Sorting and reversing a typelist

template<typename T, typename T0 = empty_typelist>
struct typelist_reverse;

template<typename T0>
struct typelist_reverse<empty_typelist, T0>
{
    using type = T0;
};

template<typename H, typename T, typename T0>
struct typelist_reverse<typelist<H,T>, T0>
{
    using type = typename typelist_reverse<T, typelist<H, T0>>::type;
};


// Sorting a typelist

template<typename Tl>
struct typelist_sort;

template<>
struct typelist_sort<empty_typelist>
{
    using type = empty_typelist;
};

template<typename H, typename T>
struct typelist_sort<typelist<H,T>>
{
    using type = typename sorted_insert<H, typename typelist_sort<T>::type>::type;
};


template<typename Tl>
struct is_sorted;

template<>
struct is_sorted<empty_typelist>
{
    static const bool value = true;
};

template<typename H>
struct is_sorted<typelist<H,empty_typelist>>
{
    static const bool value = true;
};

template<typename H1, typename H2, typename T>
struct is_sorted<typelist<H1,typelist<H2, T>>>
{
    static const bool value = compare<H1,H2>::less && is_sorted<typelist<H2, T>>::value;
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

// Start of typelist testcasdes

template<int N1, int N2, typename Tail = empty_typelist>
struct build_typelist1
{
    static const int Mid = (N1+N2)/2;
    using T0 = typename build_typelist1<N1, Mid, Tail>::type;
    using type = typename build_typelist1<Mid, N2, T0>::type;
};

template<int N, typename Tail>
struct build_typelist1<N,N+1,Tail>
{
    using type = typelist<token<N>, Tail>;
};

template<int N, typename Tail>
struct build_typelist1<N,N,Tail>
{
    using type = Tail;
};



template<int N>
struct build_typelist
{
    using P = typename build_typelist<N-1>::type;
    using type = typename typelist_insert<token<1000-N>, P>::type;
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
//    using S = typename build_typelist<N>::type;
    using S = typename build_typelist1<0,N>::type;
//    using S1 = typename typelist_reverse<S>::type;  
    using S2 = typename typelist_sort<S>::type;
    static_assert(is_sorted<S2>::value, "Typelist is not sorted");
    static_assert(typelist_size<S2>::value == N, "Typelist has the wrong size");
    static const int value = typelist_size<S2>::value; 
};

using test2 = tl_testcase<200>;

struct empty_tree;

template<typename Item, typename left, typename right>
struct type_tree;

//template<int Id>
//constexpr int hash(token<Id>) { return Id; }

template<typename H>
struct hash;

template<int Id>
struct hash<token<Id>>
{
    static const int value = Id;
};


/*
    This is a weird made-up algorithm for representing a set of items.
    We use a binary tree (already shown to be more efficient than variadic templates),
    but we use the bits of a hash to locate each item.
    The idea is that the tree structure is independent of insertion order.
    Furthermore, the node with the lower hash is placed closer to the root than nodes with higher hashes.
*/
template<typename Item, typename H, typename L, typename R, int Hash, bool Parity = Hash&1, bool ItemTop = (hash<Item>::value<hash<H>::value)>
struct hash_insert2;

template<typename Item, typename T, int Hash = hash<Item>::value>
struct hash_insert
{
    using type = typename hash_insert2<Item, T, empty_tree, empty_tree, Hash>::type;
};

template<typename Item, int Hash>
struct hash_insert<Item, empty_tree, Hash>
{
    using type = Item;
};


template<typename Item, typename H, typename L, typename R, int Hash>
struct hash_insert2<Item, H, L, R, Hash, false, false>
{
    // Keep H on the top, push Item left
    using type = type_tree<H, typename hash_insert<Item, L, (Hash>>1)>::type, R>;
};

template<typename Item, typename H, typename L, typename R, int Hash>
struct hash_insert2<Item, H, L, R, Hash, true, false>
{
    // Keep H on the top, push Item right
    using type = type_tree<H, L, typename hash_insert<Item, R, (Hash>>1)>::type>;
};

template<typename Item, typename H, typename L, typename R, int Hash>
struct hash_insert2<Item, H, L, R, Hash, false, true>
{
    // Push Item on top, with H on the left
    using type = type_tree<Item, typename hash_insert<H, L, (Hash>>1)>::type, R>;
};

template<typename Item, typename H, typename L, typename R, int Hash>
struct hash_insert2<Item, H, L, R, Hash, true, true>
{
    // Push Item on top, with H on the right
    using type = type_tree<Item, L, typename hash_insert<H, R, (Hash>>1)>::type>;
};

template<typename Item, typename H, typename L, typename R, int Hash>
struct hash_insert<Item, type_tree<H, L, R>, Hash>
{
    using type = typename hash_insert2<Item, H, L, R, Hash>::type;
};


template<typename Item, typename T, int Hash = hash(Item()), bool Parity = Hash&1>
struct hash_contains;


int main()
{
    test2::S();
    std::cout << "typelist size = " << test2::value << std::endl;
    //profile_template<test1>();
    //std::cout << "Number of typeset<> = " << static_count<typeset_tag>() << std::endl;
}
