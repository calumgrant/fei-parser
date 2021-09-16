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



template<typename Item, typename Tl>
struct typelist_contains;

template<typename Item, typename Tl, bool Contains = typelist_contains<Item, Tl>::value>
struct typelist_insert
{
    using type = Tl;
};

template<typename Tl>
struct typelist_size;



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


template<typename T>
struct tree_size
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


// Loops all elements in a non-linear order
// Used for creating tree test-cases that aren't too linear.
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
struct type_equals2
{
    static const bool value = false;
};

template<typename T>
struct type_equals2<T,T>
{
    static const bool value = true;
};

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

template<typename H, typename L, typename R, int Index>
struct make_balanced_subtree<type_tree<H, L, R>, Index, Index>
{
    using type = empty_tree;
};


template<int From, int To>
struct make_balanced_subtree<empty_tree, From, To>
{
    using type = empty_tree;
};

template<typename H, typename L, typename R, int From, int To, 
    bool OnlyInLeft = (To < (tree_size<L>::value)), 
    bool OnlyInRight = (From> (tree_size<L>::value))>
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
    bool OnlyInleft = (Element < tree_size<L>::value), bool OnlyInRight = (Element > tree_size<L>::value)>
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
    static_assert(From<To, "Invalid subtree");
    static const int mid = (To+From)/2;
    using T1 = typename make_balanced_subtree<type_tree<H,L,R>, From, mid>::type;
    using T2 = typename make_balanced_subtree<type_tree<H, L, R>, mid+1, To>::type;
    using Item = typename tree_element<type_tree<H, L, R>, mid>::type;
    using type = type_tree<Item, T1, T2>;
};

template<typename H, typename L, typename R, int N>
struct make_balanced_subtree2<H, L, R, N, N+1, false, false>
{
    using type = typename tree_element<type_tree<H, L, R>, N>::type;
};


template<typename H, typename L, typename R, int From, int To>
struct make_balanced_subtree<type_tree<H, L, R>, From, To>
{
    using type = typename make_balanced_subtree2<H, L, R, From, To>::type;
};

/*
    Creates a fully balanced tree from a type_tree.
    This is done to find "equal" sets.
 */
template<typename T>
struct make_balanced_tree
{
    using type = typename make_balanced_subtree<T, 0, tree_size<T>::value>::type;
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

    using I5 = typename tree_element<T0, 5>::type;
    static_assert(type_equals2<I5, token<5>>::value, "tree_element failed");

    using B = typename make_balanced_tree<T0>::type;

    static void output()
    {
        ::output<T0>::write(std::cout);
        std::cout << "\n\nBalanced = ";
        ::output<B>::write(std::cout);
    }
};

using test4 = treetest<2000>;

int main()
{
    test4::output();
}
