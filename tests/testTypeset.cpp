#include <cellar/cellar.hpp>
#include <simpletest.hpp>

using namespace cellar;

template<int X> struct Int {};

using E = typeset<>;
using E1 = typeset<Int<1>>;
using E1 = typename insert<Int<1>, E>::type;
using E1 = typename insert<Int<1>, E1>::type;
using E12 = typename insert<Int<2>, E1>::type;
using E12 = typename insert<Int<1>, E12>::type;
using E12 = typename insert<Int<2>, E12>::type;

using E12 = typename merge<E12, E12>::type;
using E12 = typename merge<E1, E12>::type;

using R1 = rule<100>;
using R2 = rule<101, token<1>, token<2>>;
struct S;
using C1 = typeset<rule_position<S, R2, 0, 0>>;
using C2 = typename closure<C1>::type;








template<typename T>
struct output;

template<int Id>
struct ::output<token<Id>>
{
    static void write(std::ostream & os) { os << Id; }
};

template<>
struct ::output<empty_tree>
{
    static void write(std::ostream & os) { os << "()"; }
};

template<typename H, typename L, typename R>
struct ::output<type_tree<H, L, R>>
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
    using profile_tag = no_tag;
    using profile_types = profile<tree_insert<token<Item>, Tree>>;
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
    using profile_types = profile<T0, B0, T1, T2, T3, mixed_loop<empty_tree, 0, N, tree_insert_test>>;

    static void output()
    {
        ::output<T0>::write(std::cout);
        std::cout << "\n\nBalanced = ";
        ::output<B0>::write(std::cout);

        // Beware - slow!
        //profile_template<treetest>();

        std::cout << "\n\nNumber of tree operations = " << static_count<tree_tag>() << std::endl;

        auto x = B0();
    }
};

using test4 = treetest<400>; 

namespace IntSet
{
    template<int H, typename L, typename R>
    struct intset;

    struct empty_intset
    {
        static const int size = 0;

        template<int I>
        struct insert
        {
            using type = intset<I, empty_intset, empty_intset>;
            using profile_tag = ints_tag;
            using profile_types = profile<type>;
        };

        template<typename S2>
        struct Union
        {
            using type = S2;
            using profile_tag = ints_tag;
            using profile_types = profile<type>;
        };

        using rebalance = empty_intset;
        using profile_tag = ints_tag;
        using profile_types = profile<>;
    };

    template<int H, typename L, typename R>
    struct intset
    {
        using profile_tag = ints_tag;
        using profile_types = profile<L, R>;

        static const int size = 1 + L::size + R::size;

        template<int I, bool Less = (I<H)>
        struct insert
        {
            using type = intset<H, typename L::template insert<I>::type, R>;
            using profile_tag = ints_tag;
            using profile_types = profile<L, R>;
        };

        template<int I>
        struct insert<I, false>
        {
            using type = intset<H, L, typename R::template insert<I>::type>;
            using profile_tag = ints_tag;
            using profile_types = profile<typename R::template insert<I>>;
        };

        template<>
        struct insert<H, false>
        {
            using type = intset;
            using profile_tag = ints_tag;
            using profile_types = profile<>;
        };

        template<typename Is>
        struct Union
        {
            using U1 = typename L::template Union<Is>::type;
            using U2 = typename R::template Union<U1>::type;
            using type = typename U2::template insert<H>::type;

            using profile_tag = ints_tag;
            using profile_types = profile<typename L::template Union<Is>, typename R::template Union<U1>, typename U2::template insert<H>>;
        };

        template<int I, bool InLeft = (I<L::size), bool InMiddle = (I==L::size)>
        struct element;

        template<int I>
        struct element<I, true, false>
        {
            static const int value = L::template element<I>::value;
            using profile_tag = ints_tag;
            using profile_types = profile<typename L::template element<I>>;
        };

        template<int I>
        struct element<I, false, true>
        {
            static const int value = H; 
            using profile_tag = ints_tag;
            using profile_types = profile<>;
        };

        template<int I>
        struct element<I, false, false>
        {
            static const int value = R::template element<I - L::size - 1>::value;
            using profile_tag = ints_tag;
            using profile_types = profile<typename R::template element<I - L::size - 1>>;
        };
    };

    template<typename T>
    struct output_ints;

    template<>
    struct output_ints<empty_intset>
    {
        static void write()
        {
            std::cout << "()";
        }
    };

    template<int H, typename L, typename R>
    struct output_ints<intset<H,L,R>>
    {
        static void write()
        {
            std::cout << "(";
            output_ints<L>::write();
            std::cout << " " << H << " ";
            output_ints<R>::write();
            std::cout << ")";
        }
    };


    template<typename Tree, int Item>
    struct int_insert_test
    {
        using type = typename Tree::template insert<Item>::type;
        using profile_tag = no_tag;
        using profile_types = profile<typename Tree::template insert<Item>, Tree>;

    };

    template<int N>
    struct inttest
    {
        using T0 = typename mixed_loop<empty_intset, 0, N, int_insert_test>::type;

        using U0 = typename T0::template Union<T0>::type;

        static void output()
        {
            output_ints<T0>::write();

            // profile_template<inttest>();
            // std::cout << "\nNumber of intset operations = " << static_count<ints_tag>() << std::endl;
        }

        using profile_tag = ints_tag;
        using profile_types = profile<mixed_loop<empty_intset, 0, N, int_insert_test>>;
    };
}

using bigints = IntSet::inttest<400>; 

namespace Ints
{
    template<int... Is> struct ints;

    template<int I, typename Ints>
    struct ints_insert;

    template<int I, typename Ints>
    struct ints_contains;

    template<int I>
    struct ints_contains<I, ints<>>
    {
        static const bool value = false;
    };

    template<int I, int... Is>
    struct ints_contains<I, ints<I, Is...>>
    {
        static const bool value = true;
    };

    template<int I, int J, int... Is>
    struct ints_contains<I, ints<J, Is...>>
    {
        static const bool value = ints_contains<I, ints<Is...>>::value;
    };

    template<typename A, typename B>
    struct concat;

    template<int...As, int...Bs>
    struct concat<ints<As...>, ints<Bs...>>
    {
        using type = ints<As..., Bs...>;
    };

    template<int I, int A0, int A1, int A2, int A3, int A4, int A5, int A6, int A7, int A8, int A9, int...Is>
    struct ints_contains<I, ints<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, Is...>>
    {
        static const bool value = I==A0 | I==A1 || I==A2 || I==A3 || I==A4 || I==A5 || I==A6 || I==A7 || I==A8 || I==A9 || 
            ints_contains<I, ints<Is...>>::value;
    };

    // Locates a position in a list; 
    template<int I, typename Ints>
    struct search;

    template<int I, int A0, int A1, int A2, int A3, int A4, int A5, int A6, int A7, int A8, int A9, int...Is>
    struct ints_insert<I, ints<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, Is...>>
    {
        using type = ints< 
            I<A0 ? I : A0,
            I<A0 ? A0 : I<A1 ? I : A1,
            I<A1 ? A1 : I<A2 ? I : A2,
            // etc
            
            Is...>;
    };

    template<int I, typename Is>
    struct ints_insert;

    template<int I, int... Is>
    struct ints_insert<I, ints<Is...>>
    {
        // Wrong but ok.
        using type = ints<I, Is...>;
    };


    struct empty_list {};

    template<int I, typename T>
    struct int_list
    {
    };

    template<int I, typename T>
    struct int_insert
    {

    };    
}

int main()
{
    test4::output();
    bigints::output();
}
