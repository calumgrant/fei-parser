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
template<typename Item, typename H, typename L, typename R, int Depth, bool ItemTop = (hash<Item>::value<hash<H>::value), bool Parity = ((ItemTop?hash<Item>::value:hash<H>::value)>>Depth)&1>
struct hash_insert2;

template<typename Item, typename T, int Depth=0>
struct hash_insert
{
    using type = typename hash_insert2<Item, T, empty_tree, empty_tree, Depth>::type;
};

template<typename Item, int Depth>
struct hash_insert<Item, empty_tree, Depth>
{
    using type = Item;
};


template<typename Item, typename H, typename L, typename R, int Depth>
struct hash_insert2<Item, H, L, R, Depth, false, false>
{
    // Keep H on the top, push Item left
    using type = type_tree<H, typename hash_insert<Item, L, Depth+1>::type, R>;
};

template<typename Item, typename H, typename L, typename R, int Depth>
struct hash_insert2<Item, H, L, R, Depth, false, true>
{
    // Keep H on the top, push Item right
    using type = type_tree<H, L, typename hash_insert<Item, R, Depth+1>::type>;
};

template<typename Item, typename H, typename L, typename R, int Depth>
struct hash_insert2<Item, H, L, R, Depth, true, false>
{
    // Push Item on top, with H on the left
    using type = type_tree<Item, typename hash_insert<H, L, Depth+1>::type, R>;
};

template<typename Item, typename H, typename L, typename R, int Depth>
struct hash_insert2<Item, H, L, R, Depth, true, true>
{
    // Push Item on top, with H on the right
    using type = type_tree<Item, L, typename hash_insert<H, R, Depth+1>::type>;
};

template<typename Item, typename H, typename L, typename R, int Depth>
struct hash_insert<Item, type_tree<H, L, R>, Depth>
{
    using type = typename hash_insert2<Item, H, L, R, Depth>::type;
};

template<typename Item, typename T, int Hash = hash(Item()), bool Parity = Hash&1>
struct hash_contains;

template<typename T> struct tree_size
{
    static const int value = 1;
};

template<> struct tree_size<empty_tree>
{
    static const int value = 0;
};

template<typename H, typename L, typename R>
struct tree_size<type_tree<H, L, R>>
{
    static const int value = 1 + tree_size<L>::value + tree_size<R>::value;
};

template<int N1, int N2, typename Tail = empty_tree>
struct build_typetree
{
    static const int Mid = (N1+N2)/2;
    using T0 = typename build_typetree<N1, Mid, Tail>::type;
    using type = typename build_typetree<Mid, N2, T0>::type;
};

template<int N, typename Tail>
struct build_typetree<N,N+1,Tail>
{
    using type = typename hash_insert<token<N>, Tail>::type;
};

template<int N, typename Tail>
struct build_typetree<N,N,Tail>
{
    using type = Tail;
};

// 
template<typename Init, int N1, int N2, template<typename Value, int Item> typename Body>
struct loop
{
    static const int Mid = (N1+N2)/2;
    using T0 = typename loop<Init, N1, Mid, Body>::type;
    using type = typename loop<T0, Mid, N2, Body>::type;
};

template<typename Init, int N, template<typename Value, int Item> typename Body>
struct loop<Init, N, N, Body>
{
    using type = Init;
};

template<typename Init, int N, template<typename Value, int Item> typename Body>
struct loop<Init, N, N+1, Body>
{
    using type = typename Body<Init, N>::type;
};


template<typename Init, int N1, int N2, template<typename Value, int Item> typename Body>
struct mixed_loop
{
    static const int Mid = (N1+N2)/2;
    using T1 = typename mixed_loop<Init, Mid, Mid+1, Body>::type;
    using T0 = typename mixed_loop<T1, N1, Mid, Body>::type;
    using type = typename mixed_loop<T0, Mid+1, N2, Body>::type;
};

template<typename Init, int N, template<typename Value, int Item> typename Body>
struct mixed_loop<Init, N, N, Body>
{
    using type = Init;
};

template<typename Init, int N, template<typename Value, int Item> typename Body>
struct mixed_loop<Init, N, N+1, Body>
{
    using type = typename Body<Init, N>::type;
};




template<typename T>
struct output;

template<int Id>
struct output<token<Id>>
{
    static void write(std::ostream & os) { os << Id; }
};

template<>
struct output<empty_tree>
{
    static void write(std::ostream & os) { os << "()"; }
};

template<typename H, typename L, typename R>
struct output<type_tree<H, L, R>>
{
    static void write(std::ostream & os)
    {
        os << "(";
        output<L>::write(os);
        os << " ";
        output<H>::write(os);
        os << " ";
        output<R>::write(os);
        os << ")";
    }
};

template<typename T1, typename T2>
struct tree_union
{
    using type = typename hash_insert<T1, T2>::type;
};

template<typename T2>
struct tree_union<empty_tree, T2>
{
    using type = T2;
};

template<typename H, typename L, typename R, typename T2>
struct tree_union<type_tree<H, L, R>, T2>
{
    using U0 = typename tree_union<L, T2>::type;
    using U1 = typename tree_union<R, U0>::type;
    using type = typename hash_insert<H, U1>::type;
};

template<typename T1, typename T2>
struct type_equals2
{
    static const bool value = false;
};

template<typename T>
struct type_equals2<T,T>
{
    static const bool value = true;
};


template<int N>
struct tree_test
{
    using T0 = typename build_typetree<0, N>::type;
    using T1 = typename build_typetree<0, N/1>::type;
    using T2 = typename build_typetree<N/1, N>::type;
    using T3 = typename tree_union<T1, T2>::type;
    // static_assert(type_equals2<T0, T3>::value, "tree union");

    static void output()
    {
        ::output<T0>::write(std::cout);
    }
};

using T1 = hash_insert<token<0>, empty_tree>::type;
using T2 = hash_insert<token<1>, T1>::type;
using T3 = hash_insert<token<1>, empty_tree>::type;
using T4 = hash_insert<token<0>, T3>::type;

static_assert(type_equals2<T2, T4>::value, "hash_insert failed");

using test3 = tree_test<30>; // 34 fais...

template<typename T>
struct rebalance
{
    typedef T type;
};

// Turn a tree into one tree of size N and one tree of size Size-N;
template<typename T, int N>
struct split_tree;

// 0 nodes
template<>
struct split_tree<empty_tree, 0>
{
    using left = empty_tree;
    using right = empty_tree;
};

template<typename T>
struct split_tree<T, 0>
{
    using left = empty_tree;
    using right = T;
};

template<typename T>
struct split_tree<T, 1>
{
    using left = T;
    using right = empty_tree;
};

template<typename H, typename L, typename R>
struct rebalance<type_tree<H, L, R>>
{
    static const int nodes_to_move = tree_size<L>::value - tree_size<R>::value;
    // Cut "N" nodes from the left
    int size1 = tree_size<L>::value;
};

template<typename Tree, typename Init, template<typename Item, typename Aggregate> typename Visitor>
struct visit
{
    // Default tree of one item
    using tree = typename Visitor<Tree, Init>::type;
};

template<typename Init, template<typename Item, typename Aggregate> typename Visitor>
struct visit<empty_tree, Init, Visitor>
{
    using tree = Init;
};

template<typename H, typename L, typename R, typename Init, template<typename Item, typename Aggregate> typename Visitor>
struct visit<type_tree<H, L, R>, Init, Visitor>
{
    using type = typename visit<R, typename Visitor<H, typename visit<L, Init, Visitor>::type>::type, Visitor>::type;
};

template<typename T, int From, int To>
struct make_balanced_subtree
{
    // Default case - T is an element
    using type = T;    
};

template<typename T, int Index>
struct make_balanced_subtree<T, Index, Index>
{
    using type = empty_tree;
};

template<int From, int To>
struct make_balanced_subtree<empty_tree, From, To>
{
    using type = empty_tree;
};

template<typename H, typename L, typename R, int From, int To, 
    bool InLeft = (To <= (tree_size<L>::value)), 
    bool InRight = (From> (tree_size<L>::value))>
struct make_balanced_subtree2;

template<typename H, typename L, typename R, int From, int To>
struct make_balanced_subtree2<H, L, R, From, To, true, false>
{
    using type = typename make_balanced_subtree<L, From, To>::type;
};

template<typename H, typename L, typename R, int From, int To>
struct make_balanced_subtree2<H, L, R, From, To, false, true>
{
    using type = typename make_balanced_subtree<R, From-tree_size<L>::value-1, To-tree_size<L>::value-1>::type;
};

// Extracts an element from a tree
template<typename T, int Element>
struct tree_element
{
    // Case of one element
    using type = T;
};

template<typename H, typename L, typename R, int Element, 
    bool Inleft = (Element < tree_size<L>::value), bool Inright = (Element > tree_size<L>::value)>
struct tree_element2;

template<typename H, typename L, typename R, int Element>
struct tree_element2<H,L,R,Element,true,false>
{
    using type = typename tree_element<L, Element>::type;
};

template<typename H, typename L, typename R, int Element>
struct tree_element2<H,L,R,Element,false,true>
{
    using type = typename tree_element<R, Element - tree_size<L>::value-1>::type;
};

template<typename H, typename L, typename R, int Element>
struct tree_element2<H,L,R,Element,false,false>
{
    using type = H;
};


template<typename H, typename L, typename R, int Element>
struct tree_element<type_tree<H, L, R>, Element>
{
    using type = typename tree_element2<H, L, R, Element>::type;
};


template<typename H, typename L, typename R, int From, int To>
struct make_balanced_subtree2<H, L, R, From, To, false, false>
{
    static const int mid = (To+From)/2;
    using T1 = typename make_balanced_subtree<type_tree<H,L,R>, From, mid-1>::type;
    using T2 = typename make_balanced_subtree<type_tree<H, L, R>, mid+1, To>::type;
    using Item = typename tree_element<type_tree<H, L, R>, mid>::type;
    using type = type_tree<Item, T1, T2>;
};

template<typename H, typename L, typename R, int From, int To>
struct make_balanced_subtree<type_tree<H, L, R>, From, To>
{
    using type = typename make_balanced_subtree2<H, L, R, From, To>::type;
};



template<typename Item, typename H, typename L, typename R, bool Less = (hash<Item>::value < hash<H>::value)>
struct tree_insert2;


template<typename Item, typename T>
struct tree_insert
{
    using type = typename tree_insert2<Item, T, empty_tree, empty_tree>::type;
};

template<typename Item>
struct tree_insert<Item, empty_tree>
{
    using type = Item;
};

template<typename Item, typename H, typename L, typename R, bool Less>
struct tree_insert2
{
    // False case - insert to the right
    using type = type_tree<H, L, typename tree_insert<Item, R>::type>;
};

template<typename Item, typename H, typename L, typename R>
struct tree_insert2<Item, H, L, R, true>
{
    // True case - insert to the left
    using type = type_tree<H, typename tree_insert<Item, L>::type, R>;
};

template<typename Item, typename L, typename R>
struct tree_insert<Item, type_tree<Item,L,R>>
{
    using type = type_tree<Item,L,R>;
};

template<typename Item, typename H, typename L, typename R>
struct tree_insert<Item, type_tree<H,L,R>>
{
    using type = typename tree_insert2<Item, H, L, R>::type;
};

template<typename Tree, int Item>
struct tree_insert_test
{
    using type = typename tree_insert<token<Item>, Tree>::type;
};

template<int N>
struct treetest
{
    using T0 = typename mixed_loop<empty_tree, 0, N, tree_insert_test>::type;
    static_assert(tree_size<T0>::value == N, "Failed tree_insert");

    static void output()
    {
        ::output<T0>::write(std::cout);
    }
};

using test4 = treetest<10000>;

int main()
{
    test2::S();
    std::cout << "typelist size = " << test2::value << std::endl;

    std::cout << "typetree size = " << tree_size<test3::T0>::value << std::endl;

    test4::output();
}
