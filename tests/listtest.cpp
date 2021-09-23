#include <simpletest.hpp>
#include "typeset_test.hpp"

namespace cellar
{
    template<typename S>
    struct size;

    template<int Min, int Max>
    struct range {};

    template<int Min, int Max>
    struct size<range<Min, Max>>
    {
        static const int value = Max-Min;
    };

    template<int Length, int Min, int Max>
    struct random_sequence {};

    template<int Min, int Max, int Length>
    struct size<random_sequence<Length, Min, Max>>
    {
        static const int value = Length;
    };

    template<int N, int A=123, int C=456, int M=789>
    struct linear_congruential
    {
        static const int value = (A * linear_congruential<N-1, A, C, M>::value + C) % M;
    };

    template<int A, int C, int M>
    struct linear_congruential<0, A, C, M>
    {
        static const int value = 789 % M;  // Or a random seed
    };

    template<typename IntSequence, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop2;

    template<int Min, int Max, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop2<range<Min, Max>, Seed, Body>
    {
        static const int Mid = (Min + Max)/2;
        using L1 = typename loop2<range<Min, Mid>, Seed, Body>::type;
        using L2 = typename Body<Mid, L1>::type;
        using type = typename loop2<range<Mid+1, Max>, L2, Body>::type;
    };

    template<int N, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop2<range<N, N>, Seed, Body>
    {
        using type = Seed;
    };

    template<int N, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop2<range<N, N+1>, Seed, Body>
    {
        using type = typename Body<N, Seed>::type;
    };

    template<int Min, int Max, int Length, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop2<random_sequence<Length, Min, Max>, Seed, Body>
    {
        static const int Mid = Length/2;
        using T0 = typename loop2<random_sequence<Length-1, Min, Max>, Seed, Body>::type;
        using type = typename Body<Min + linear_congruential<Length>::value % (Max-Min+1), T0>::type;
    };

    template<int Min, int Max, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop2<random_sequence<0, Min, Max>, Seed, Body>
    {
        using type = Seed;
    };

    // Constructors:
    /*
        make_range<>
        make_random<0, 100, 100>
        make_set<>
    */

    template<int Length, int Min, int Max>
    struct make_random
    {
        using type = random_sequence<Length, Min, Max>;
    };

    template<int Min, int Max>
    struct make_range
    {
        using type = range<Min, Max>;
    };

    struct empty_node;

    template<typename H, typename T>
    struct list_node;

    template<typename...Ts>
    struct make_list;

    template<>
    struct make_list<>
    {
        using type = empty_node;
    };

    template<typename Item, typename Collection>
    struct insert;

    template<typename Item>
    struct insert<Item, empty_node>
    {
        using type = list_node<Item, empty_node>;
    };

    template<typename Item, typename T>
    struct insert<Item, list_node<Item, T>>
    {
        using type = list_node<Item, T>;
    };

    template<typename Item, typename H, typename T, bool Less = (hash<Item>::value < hash<H>::value)>
    struct list_insert
    {
        // true case: Item < H
        using type = list_node<Item, list_node<H,T>>;
    };

    template<typename Item, typename H, typename T>
    struct list_insert<Item, H, T, false>
    {
        // true case: Item < H
        using type = list_node<H, typename insert<Item, T>::type>;
    };

    template<typename Item, typename H, typename T>
    struct insert<Item, list_node<H, T>>
    {
        using type = typename list_insert<Item, H, T>::type;
    };


    template<typename T, typename...Ts>
    struct make_list<T, Ts...>
    {
        using type = typename insert<T, typename make_list<Ts...>::type>::type;
    };

    template<>
    struct size<empty_node>
    {
        static const int value = 0;
    };

    template<typename H, typename T>
    struct size<list_node<H,T>>
    {
        static const int value = 1 + size<T>::value;
    };

    template<typename T>
    struct output;

    template<>
    struct output<empty_node>
    {
        static void write(std::ostream & os)
        {
            os << "{}";
        }
    };

    template<typename T>
    struct visitor;

    template<>
    struct visitor<empty_node>
    {
        template<typename Visitor, typename...Args>
        static void visit(Args&&...args)
        {
            // Nothing to visit.
        }
    };

    template<typename H, typename T>
    struct visitor<list_node<H,T>>
    {
        template<typename Visitor, typename...Args>
        static void visit(Args&&...args)
        {
            Visitor::template visit<H, T>(std::forward<Args&&...>(args...));
        }
    };

    struct output_visitor
    {
        template<typename Item, typename Next> static void visit(std::ostream & os)
        {
            os << Item();
            if(!type_equals<Next, empty_node>::value) os << ",";
            visitor<Next>::template visit<output_visitor>(os);
        }
    };

    template<typename H, typename T>
    struct output<list_node<H,T>>
    {
        static void write(std::ostream & os)
        {
            os << "{";

            visitor<list_node<H,T>>::template visit<output_visitor>(os);
            // TODO: Something with iterators

            os << "}";
        }
    };

    template<typename T> void write(std::ostream &os)
    {
        output<T>::write(os);
        std::cout << std::endl;
    }

    template<typename T1, typename T2>
    struct merge;

    template<>
    struct merge<empty_node, empty_node>
    {
        using type = empty_node;
    };

    template<typename T>
    struct merge<empty_node, T>
    {
        using type = T;
    };

    template<typename T>
    struct merge<T, empty_node>
    {
        using type = T;
    };

    template<typename H1, typename T1, typename H2, typename T2, bool Less = (hash<H1>::value < hash<H2>::value)>
    struct list_merge
    {
        // true case
        // H1 goes on first
        using type = list_node<H1, typename merge<T1, list_node<H2, T2>>::type>;
    };

    template<typename H1, typename T1, typename H2, typename T2>
    struct list_merge<H1, T1, H2, T2, false>
    {
        // false case
        // H2 goes first

        using type = list_node<H2, typename merge<list_node<H1, T1>, T2>::type>;
    };


    template<typename H, typename T1, typename T2>
    struct list_merge<H, T1, H, T2, false>
    {
        // Only one goes on top
        using type = list_node<H, typename merge<T1, T2>::type>;
    };


    template<typename H1, typename T1, typename H2, typename T2>
    struct merge<list_node<H1, T1>, list_node<H2, T2>>
    {
        using type = typename list_merge<H1, T1, H2, T2>::type;
    };

    // !! Contains

    template<typename Item, typename List>
    struct contains;

    template<typename Item>
    struct contains<Item, empty_node>
    {
        static const bool value = false;
    };

    template<typename Item, typename H, typename T, bool Less = (hash<Item>::value < hash<H>::value)>
    struct list_contains
    {
        // False case - go to next item.
        static const bool value = contains<Item, T>::value;
    };

    template<typename Item, typename T>
    struct list_contains<Item, Item, T, false>
    {
        static const bool value = true;
    };

    template<typename Item, typename H, typename T>
    struct list_contains<Item, H, T, true>
    {
        // True case: Item not in list
        static const bool value = false;
    };

    template<typename Item, typename H, typename T>
    struct contains<Item, list_node<H, T>>
    {
        static const bool value = list_contains<Item, H, T>::value;
    };

}


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
    using T5 = typename loop2<make_range<0, 100>::type, List, build_list>::type;
    using T6 = typename loop2<make_random<500, 0, 1000>::type, List, build_list>::type;

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
