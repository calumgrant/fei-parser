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
    static_assert(type_equals<I5, token<2>>::value, "tree_element failed");

    static_assert(tree_contains<token<2>, T0>::value, "tree contains 5");

    using B0 = typename make_balanced_tree<T0>::type;

    using T1 = typename mixed_loop<empty_tree, 0, N/2, tree_insert_test>::type;
    using T2 = typename mixed_loop<empty_tree, N/2, N, tree_insert_test>::type;
    using T3 = typename tree_union<T1,T1>::type;

    static_assert(type_equals<T1, T3>::value, "tree_union failed");

    //using T4 = typename tree_union<T1,T2>::type;
    //using B4 = typename make_balanced_tree<T4>::type;

    //static_assert(type_equals2<B0, B4>::value, "tree_union failed");

    using profile_tag = no_tag;
    using profile_types = profile<T0, B0, T1, T2, T3>;

    static void output()
    {
        ::output<T0>::write(std::cout);
        std::cout << "\n\nBalanced = ";
        ::output<B0>::write(std::cout);

        profile_template<treetest>();
        std::cout << "\n\nNumber of tree operations = " << static_count<tree_tag>() << std::endl;

        auto x = B0();
    }
};

using test4 = treetest<1050>;

int main()
{
    test4::output();
}
