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


namespace cellar
{
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

template<typename Init, int N1, int N2, template<typename Value, int Item> typename Body>
struct loop
{
    static_assert(N1<N2, "Invalid bounds for loop");
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
    static_assert(N1<N2, "Invalid bounds for loop");
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

    template<>
    struct make_balanced_subtree<empty_tree, 0, 0>
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
struct tree_insert<Item, Item>
{
    using type = Item;
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

template<typename Item, typename T>
struct tree_contains
{
    static const bool value = false;
};

template<typename Item>
struct tree_contains<Item, empty_tree>
{
    static const bool value = false;
};

template<typename Item>
struct tree_contains<Item, Item>
{
    static const bool value = true;
};

template<typename Item, typename L, typename R>
struct tree_contains<Item, type_tree<Item, L, R>>
{
    static const bool value = true;
};

template<typename Item, typename H, typename L, typename R, bool Less = (hash<Item>::value < hash<H>::value)>
struct tree_contains2
{
    // true case: Search left subtree
    static const bool value = tree_contains<Item, L>::value;
};

template<typename Item, typename H, typename L, typename R>
struct tree_contains2<Item, H, L, R, false>
{
    // false case: Search right subtree
    static const bool value = tree_contains<Item, R>::value;
};

template<typename Item, typename H, typename L, typename R>
struct tree_contains<Item, type_tree<H, L, R>>
{
    static const bool value = tree_contains2<Item, H, L, R>::value;
};


template<typename T1, typename T2>
struct tree_union
{
    using type = typename tree_insert<T1, T2>::type;
};

template<typename T2>
struct tree_union<empty_tree, T2>
{
    using type = T2;
};

template<typename H, typename L, typename R, typename T2>
struct tree_union<type_tree<H, L, R>, T2>
{
    using T0 = typename tree_union<L, T2>::type;
    using T1 = typename tree_union<R, T0>::type;
    using type = typename tree_insert<H, T1>::type;  
};


template<typename Tree>
struct typeset2
{
    using tree = Tree;

    static const int size = tree_size<Tree>::value;

    template<typename Item>
    struct insert
    {
        using type = typeset2<typename tree_insert<Item, Tree>::type>;
    };

    template<typename Item>
    struct contains
    {
        static const bool value = tree_contains<Item, Tree>::value;
    };

    template<typename T2>
    struct set_union
    {
            using type = typeset2<typename tree_union<Tree, typename T2::tree>::type>;
    };

    using balanced = typeset2<typename make_balanced_tree<Tree>::type>;

    template<typename Start, template<typename Value, typename Item> typename Visitor>
    struct visit
    {
        using type = typename ::visit<Tree, Start, Visitor>::type;
    };

        template<int Element>
        struct element
        {
            using type = typename tree_element<Tree, Element>::type;
        };
    };

    using empty_typeset2 = typeset2<empty_tree>;
}

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

    using I5 = typename tree_element<T0, 2>::type;
    static_assert(type_equals2<I5, token<2>>::value, "tree_element failed");

    static_assert(tree_contains<token<2>, T0>::value, "tree contains 5");

    using B0 = typename make_balanced_tree<T0>::type;

    using T1 = typename mixed_loop<empty_tree, 0, N/2, tree_insert_test>::type;
    using T2 = typename mixed_loop<empty_tree, N/2, N, tree_insert_test>::type;
    using T3 = typename tree_union<T1,T1>::type;

    static_assert(type_equals2<T1, T3>::value, "tree_union failed");

    //using T4 = typename tree_union<T1,T2>::type;
    //using B4 = typename make_balanced_tree<T4>::type;

    //static_assert(type_equals2<B0, B4>::value, "tree_union failed");


    static void output()
    {
        ::output<T0>::write(std::cout);
        std::cout << "\n\nBalanced = ";
        ::output<B0>::write(std::cout);
    }
};

using test4 = treetest<2500>;

int main()
{
    test4::output();
}
