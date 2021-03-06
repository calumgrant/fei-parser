#pragma once

#include "hash.hpp"

namespace cellar
{
    template<typename...Members>
    struct typeset;

    template<typename Item, typename Ts>
    struct contains;

    template<typename Item, typename Ts>
    struct insert;

    template<typename...Members>
    struct typeset
    {
        using profile_tag = typeset_tag;
        using profile_types = profile<Members...>;
    };

    template<typename Set, typename Item>
    struct insert;

    template<typename Item>
    struct insert<Item, typeset<>>
    {
        typedef typeset<Item> type;
        using profile_tag = insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename...Members>
    struct insert<Item, typeset<Item, Members...>>
    {
        typedef typeset<Item, Members...> type;

        using profile_tag = insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename TypeSet> struct typeset_ins {};

    template<typename Item, typename... Members>
    struct typeset_ins<Item, typeset<Members...>>
    {
        typedef typeset<Item, Members...> type;
        using profile_tag = insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename Item2, typename...Members>
    struct insert<Item, typeset<Item2, Members...>>
    {
        using S = typename insert<Item, typeset<Members...>>::type;
        typedef typename typeset_ins<Item2, S>::type type;

        using profile_tag = insert_tag;
        using profile_types = profile<insert<Item, typeset<Members...>>, typeset_ins<Item2, S>, type>;
    };

    template<typename T1, typename T2>
    struct merge;

    template<typename T2>
    struct merge<typeset<>, T2>
    {
        typedef T2 type;
    };

    template<typename Item, typename...Members, typename T2>
    struct merge<typeset<Item, Members...>, T2>
    {
        using S = typename merge<typeset<Members...>, T2>::type;
        using type = typename insert<Item, S>::type;

        using profile_tag = merge_tag;
        using profile_types = profile<type, merge<typeset<Members...>, T2>, insert<Item, S>>;
    };

    template<typename Ts>
    struct size;

    template<>
    struct size<typeset<>>
    {
        static const int value = 0;
        using profile_tag = size_tag;
        using profile_types = profile<typeset<>>;
    };

    template<typename Item, typename... Members>
    struct size<typeset<Item, Members...>>
    {
        static const int value = 1 + size<typeset<Members...>>::value;
        using profile_tag = size_tag;
        using profile_types = profile<size<typeset<Members...>>>;
    };

    template<typename Item, typename Ts>
    struct contains;

    template<typename Item>
    struct contains<Item, typeset<>>
    {
        static const bool value = false;
        using profile_tag = contains_tag;
        using profile_types = profile<typeset<>>;
    };

    template<typename Item>
    struct contains<Item, typeset<Item>>
    {
        static const bool value = true;
        using profile_tag = contains_tag;
        using profile_types = profile<Item, typeset<Item>>;
    };

    template<typename Item, typename I0>
    struct contains<Item, typeset<I0>>
    {
        static const bool value = false;
        using profile_tag = contains_tag;
        using profile_types = profile<Item, typeset<I0>>;
    };

    template<typename Item, typename I0, typename I1, typename... Items>
    struct contains<Item, typeset<I0, I1, Items...>>
    {
        static const bool value = contains<Item, typeset<Items...>>::value;
        using profile_tag = contains_tag;
        using profile_types = profile<Item, typeset<I0, I1, Items...>, contains<Item, typeset<Items...>>>;
    };

    template<typename Item, typename I1, typename... Items>
    struct contains<Item, typeset<Item, I1, Items...>>
    {
        static const bool value = true;
        using profile_tag = contains_tag;
        using profile_types = profile<Item, typeset<Item, I1, Items...>>;
    };

    template<typename Item, typename I0, typename... Items>
    struct contains<Item, typeset<I0, Item, Items...>>
    {
        static const bool value = true;
        using profile_tag = contains_tag;
        using profile_types = profile<Item, typeset<I0, Item, Items...>>;
    };

/*
    template<typename Item, typename I0, typename I1, typename I2, typename...Items>
    struct contains<Item, typeset<I0, I1, I2>>
    {

    };
*/

    template<
        typename Ts, 
        typename Init,
        template<typename Item, typename Accumulator> typename Aggregate
        >
    struct typeset_aggregate;

    template<
        typename Init,
        template<typename Item, typename Accumulator> typename Aggregate
        >
    struct typeset_aggregate<typeset<>, Init, Aggregate>
    {
        using type = Init;
    };

    template<
        typename Item1,
        typename ...Items,
        typename Init, template<typename Item, typename Accumulator> typename Aggregate
        >
    struct typeset_aggregate<typeset<Item1, Items...>, Init, Aggregate>
    {
        using T0 = typename typeset_aggregate<typeset<Items...>, Init, Aggregate>::type;
        using type = typename Aggregate<Item1, T0>::type;
    };

    struct typeset_subset_tag;

    template<typename Ts1, typename Ts2>
    struct typeset_subset;

    template<typename Ts2>
    struct typeset_subset<typeset<>, Ts2>
    {
        static const bool value = true;
        using profile_tag = typeset_subset_tag;
        using profile_types = profile<typeset<>, Ts2>;
    };

    template<typename Item, typename...Items, typename Ts2>
    struct typeset_subset<typeset<Item, Items...>, Ts2>
    {
        static const bool value = contains<Item, Ts2>::value &&
            typeset_subset<typeset<Items...>, Ts2>::value;

        using profile_tag = typeset_subset_tag;
        using profile_types = profile<
            typeset<Item, Items...>,
            Ts2,
            contains<Item, Ts2>,
            typeset_subset<typeset<Items...>, Ts2>>;
    };

    template<typename Ts1, typename Ts2>
    struct typeset_equals
    {
        static const bool value = typeset_subset<Ts1, Ts2>::value && typeset_subset<Ts2, Ts1>::value;
        using profile_tag = no_tag;
        using profile_types = profile<Ts1, Ts2, typeset_subset<Ts1, Ts2>, typeset_subset<Ts2, Ts1>>;
    };

    constexpr typeset<> empty_ts() { return typeset<>(); }

    constexpr bool is_empty(typeset<>) { return true; }

    template<typename Item, typename...Items>
    constexpr bool is_empty(typeset<Item, Items...>) { return false; }

    template<typename... Ts1, typename... Ts2>
    constexpr bool operator<=(typeset<Ts1...> t1, typeset<Ts2...> t2)
    {
        return typeset_subset<typeset<Ts1...>, typeset<Ts2...>>::value;
    }

    template<typename... Ts1, typename... Ts2>
    constexpr bool operator==(typeset<Ts1...> t1, typeset<Ts2...> t2)
    {
        return t1<=t2 && t2<=t1;
    }

    template<typename... Ts1, typename... Ts2>
    constexpr auto operator+(typeset<Ts1...> t1, typeset<Ts2...> t2)
    {
        return merge<typeset<Ts1...>, typeset<Ts2...>>::type();
    }
}





namespace cellar
{
    struct empty_tree
    {
        using profile_tag = tree_tag;
        using profile_types = profile<>;
    };

    /*
        A binary tree of types (not values)
        Items are compared using `hash<X>::value < hash<Y>::value` - which assumes that all items
        have a hash. This is actually true for cellar. Hash collisions are fine however.

        This design is the result of experimentation to find an effient approach that the
        compiler does not choke on. The naive implementation using variadic templates for example
        does not perform very well, and the type-list design runs into template depth limits of
        1024. Various other algorithms that would naturally be linear (such as the `loop` template)
        are based on binary splitting to work around template-recursion limits.

        The current design scales to tens of thousands of elements.

        `Left` and `Right` can be items in the tree, and are only interpreted as 
        tree nodes if they have the type `type_tree` or `empty_tree`. This is more efficient. But as a result
        most algorithms need to handle 3 cases: `empty_tree`, single value, and `type_tree`

        The general principles for efficiency are:
        - Reduce the number of unique types
        - Reuse types wherever possible
        - Small types are better
        - Avoid decorations
        - Divide and conquer
        - Avoid linearity
        - Prefer wide rather than deep recursion
        - Split cases using bools to instantiate only the case you need
        - Find a normal form (`make_balanced_tree`/`normalize`) to avoid duplicating identical cases with different type structure.
    */
    template<typename Item, typename Left, typename Right>
    struct type_tree
    {
        using profile_tag = tree_tag;
        using profile_types = profile<Item, Left, Right>;
    };

    template<typename H>
    struct hash;


    // Computes the number of nodes in a tree
    template<typename T>
    struct size
    {
        // Default case - the tree is a single item.
        static const int value = 1;
    };

    template<> struct size<empty_tree>
    {
        static const int value = 0;
    };

    template<typename H, typename L, typename R>
    struct size<type_tree<H, L, R>>
    {
        static const int value = 1 + size<L>::value + size<R>::value;
    };

    template<typename Tree, typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall
    {
        // Default tree of one item
        using type = typename Visitor<Tree, Init>::type;
        using profile_tag = no_tag;
        using profile_types = profile<Tree, Init, Visitor<Tree, Init>>;
    };

    template<typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<empty_tree, Init, Visitor>
    {
        using type = Init;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename H, typename L, typename R, typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<type_tree<H, L, R>, Init, Visitor>
    {
        using type = typename forall<R, typename Visitor<H, typename forall<L, Init, Visitor>::type>::type, Visitor>::type;

        using profile_tag = no_tag;
        using profile_types = profile<type_tree<H, L, R>, Init, forall<R, typename Visitor<H, typename forall<L, Init, Visitor>::type>::type, Visitor>>;
    };

    template<typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<typeset<>, Init, Visitor>
    {
        using type = Init;

        using profile_tag = no_tag;
        using profile_types = profile<Init>;
    };

    template<typename I, typename...Is, typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<typeset<I, Is...>, Init, Visitor>
    {
        using T = typename Visitor<I, Init>::type;
        using type = typename forall<typeset<Is...>, T, Visitor>::type;

        using profile_tag = no_tag;
        using profile_types = profile<Visitor<I, Init>, Init, forall<typeset<Is...>, T, Visitor>>;
    };

    // Extracts an element from a tree
    template<typename T, int Element>
    struct element
    {
        // Case where the tree consists of one element
        using type = T;
        using profile_tag = tree_tag;
        using profile_types = profile<>;
    };

    // Extracts an element from a type_tree.
    // There are three cases:
    //   - element is in the left (`InLeft=true`),
    //   - element is in the right (`InRight=true`) or
    //   - element is at the root
    template<typename H, typename L, typename R, int Element, 
        bool InLeft = (Element < size<L>::value), bool InRight = (Element > size<L>::value)>
    struct element2;

    template<typename H, typename L, typename R, int Element>
    struct element2<H,L,R,Element,true,false>
    {
        using type = typename element<L, Element>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<element<L, Element>>;
    };

    template<typename H, typename L, typename R, int Element>
    struct element2<H,L,R,Element,false,true>
    {
        using type = typename element<R, Element - size<L>::value-1>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<element<R, Element - size<L>::value-1>>;
    };

    template<typename H, typename L, typename R, int Element>
    struct element2<H,L,R,Element,false,false>
    {
        using type = H;
        using profile_tag = tree_tag;
        using profile_types = profile<>;
    };

    template<typename H, typename L, typename R, int Element>
    struct element<type_tree<H, L, R>, Element>
    {
        using type = typename element2<H, L, R, Element>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<element2<H, L, R, Element>>;
    };

    template<typename T, int From, int To>
    struct make_balanced_subtree
    {
        // Default case - T is an element
        using type = T;
        using profile_tag = tree_tag;
        using profile_types = profile<>;    
    };

    template<typename T, int Index>
    struct make_balanced_subtree<T, Index, Index>
    {
        using type = empty_tree;
        using profile_tag = tree_tag;
        using profile_types = profile<>;
    };

    template<typename H, typename L, typename R, int Index>
    struct make_balanced_subtree<type_tree<H, L, R>, Index, Index>
    {
        using type = empty_tree;
        using profile_tag = tree_tag;
        using profile_types = profile<>;
    };

    template<typename H, typename L, typename R, int From, int To, 
        bool OnlyInLeft = (To < (size<L>::value)), 
        bool OnlyInRight = (From> (size<L>::value))>
    struct make_balanced_subtree2;

    template<typename H, typename L, typename R, int From, int To>
    struct make_balanced_subtree2<H, L, R, From, To, true, false>
    {
        using type = typename make_balanced_subtree<L, From, To>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<
            make_balanced_subtree<L, From, To>
            >;
    };

    template<typename H, typename L, typename R, int From, int To>
    struct make_balanced_subtree2<H, L, R, From, To, false, true>
    {
        using type = typename make_balanced_subtree<R, From-size<L>::value-1, To-size<L>::value-1>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<
            make_balanced_subtree<R, From-size<L>::value-1, To-size<L>::value-1>
            >;
    };


    template<typename H, typename L, typename R, int From, int To>
    struct make_balanced_subtree2<H, L, R, From, To, false, false>
    {
        static_assert(From<To, "Invalid subtree");
        static const int mid = (To+From)/2;
        using T1 = typename make_balanced_subtree<type_tree<H,L,R>, From, mid>::type;
        using T2 = typename make_balanced_subtree<type_tree<H, L, R>, mid+1, To>::type;
        using Item = typename element<type_tree<H, L, R>, mid>::type;
        using type = type_tree<Item, T1, T2>;
        using profile_tag = tree_tag;
        using profile_types = profile<
            make_balanced_subtree<type_tree<H,L,R>, From, mid>,
            make_balanced_subtree<type_tree<H, L, R>, mid+1, To>,
            element<type_tree<H, L, R>, mid>,
            type
            >;
    };

    template<typename H, typename L, typename R, int N>
    struct make_balanced_subtree2<H, L, R, N, N+1, false, false>
    {
        using type = typename element<type_tree<H, L, R>, N>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<element<type_tree<H, L, R>, N>>;
    };

    template<typename H, typename L, typename R, int From, int To>
    struct make_balanced_subtree<type_tree<H, L, R>, From, To>
    {
        using type = typename make_balanced_subtree2<H, L, R, From, To>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<make_balanced_subtree2<H, L, R, From, To>>;
    };

    /*
        Creates a fully balanced tree from a type_tree.
        This is done to find "equal" sets.
    */
    template<typename T>
    struct make_balanced_tree
    {
        using type = typename make_balanced_subtree<T, 0, size<T>::value>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<make_balanced_subtree<T, 0, size<T>::value>>;
    };


    template<typename Item, typename H, typename L, typename R, bool Less = (hash<Item>::value < hash<H>::value)>
    struct insert2;


    template<typename Item, typename T>
    struct insert
    {
        using type = typename insert2<Item, T, empty_tree, empty_tree>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, T, insert2<Item, T, empty_tree, empty_tree>>;
    };

    template<typename Item>
    struct insert<Item, Item>
    {
        using type = Item;
        using profile_tag = tree_tag;
        using profile_types = profile<Item>;
    };

    template<typename H, typename L, typename R>
    struct insert<type_tree<H, L, R>, empty_tree>
    {
        // This special case is when you store a tree in a tree: The values nodes could get
        // confused with subtrees if we don't do this.
        using type = type_tree<type_tree<H,L,R>, empty_tree, empty_tree>;
        using profile_tag = tree_tag;
        using profile_types = profile<type>;
    };


    template<typename Item>
    struct insert<Item, empty_tree>
    {
        using type = Item;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, empty_tree>;
    };

    template<typename Item, typename H, typename L, typename R, bool Less>
    struct insert2
    {
        // False case - insert to the right
        using type = type_tree<H, L, typename insert<Item, R>::type>;
        using profile_tag = tree_tag;
        using profile_types = profile<insert<Item, R>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct insert2<Item, H, L, R, true>
    {
        // True case - insert to the left
        using type = type_tree<H, typename insert<Item, L>::type, R>;
        using profile_tag = tree_tag;
        using profile_types = profile<insert<Item, L>>;
    };

    template<typename Item, typename L, typename R>
    struct insert<Item, type_tree<Item,L,R>>
    {
        using type = type_tree<Item,L,R>;
        using profile_tag = tree_tag;
        using profile_types = profile<type_tree<Item,L,R>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct insert<Item, type_tree<H,L,R>>
    {
        using type = typename insert2<Item, H, L, R>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, type_tree<H,L,R>, insert2<Item, H, L, R>>;
    };

    template<typename Item, typename T>
    struct contains
    {
        static const bool value = false;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, T>;
    };

    template<typename Item>
    struct contains<Item, empty_tree>
    {
        static const bool value = false;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, empty_tree>;
    };

    template<typename Item>
    struct contains<Item, Item>
    {
        static const bool value = true;
        using profile_tag = tree_tag;
        using profile_types = profile<Item>;
    };

    template<typename Item, typename L, typename R>
    struct contains<Item, type_tree<Item, L, R>>
    {
        static const bool value = true;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, L, R>;
    };

    template<typename Item, typename H, typename L, typename R, bool Less = (hash<Item>::value < hash<H>::value)>
    struct contains2
    {
        // true case: Search left subtree
        static const bool value = contains<Item, L>::value;
        using profile_tag = tree_tag;
        using profile_types = profile<contains<Item, L>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct contains2<Item, H, L, R, false>
    {
        // false case: Search right subtree
        static const bool value = contains<Item, R>::value;
        using profile_tag = tree_tag;
        using profile_types = profile<contains<Item, R>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct contains<Item, type_tree<H, L, R>>
    {
        static const bool value = contains2<Item, H, L, R>::value;
        using profile_tag = tree_tag;
        using profile_types = profile<H, Item, L, R, contains2<Item, H, L, R>>;
    };


    template<typename T1, typename T2>
    struct merge
    {
        using type = typename insert<T1, T2>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<insert<T1, T2>, type>;
    };

    template<typename T2>
    struct merge<empty_tree, T2>
    {
        using type = T2;
        using profile_tag = tree_tag;
        using profile_types = profile<type>;
    };

    template<typename H, typename L, typename R, typename T2>
    struct merge<type_tree<H, L, R>, T2>
    {
        using T0 = typename merge<L, T2>::type;
        using T1 = typename merge<R, T0>::type;
        using type = typename insert<H, T1>::type;  

        using profile_tag = tree_tag;
        using profile_types = profile<H, L, R, merge<L, T2>, merge<R, T0>, type>;
    };

    template<typename T1, typename T2>
    struct tree_equals
    {
        static const bool value = type_equals<typename make_balanced_tree<T1>::type, typename make_balanced_tree<T2>::type>::value;
    };
}


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

    template<int N, int A=123, int C=456, int M=0x10000>
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
    struct loop;

    template<int Min, int Max, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop<range<Min, Max>, Seed, Body>
    {
        static const int Mid = (Min + Max)/2;
        using L1 = typename loop<range<Min, Mid>, Seed, Body>::type;
        using L2 = typename Body<Mid, L1>::type;
        using type = typename loop<range<Mid+1, Max>, L2, Body>::type;
    };

    template<int N, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop<range<N, N>, Seed, Body>
    {
        using type = Seed;
    };

    template<int N, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop<range<N, N+1>, Seed, Body>
    {
        using type = typename Body<N, Seed>::type;
    };

    template<int Min, int Max, int Length, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop<random_sequence<Length, Min, Max>, Seed, Body>
    {
        static const int Mid = Length/2;
        using T0 = typename loop<random_sequence<Length-1, Min, Max>, Seed, Body>::type;
        using type = typename Body<Min + linear_congruential<Length>::value % (Max-Min+1), T0>::type;
    };

    template<int Min, int Max, typename Seed, template<int Item, typename Aggregate> typename Body>
    struct loop<random_sequence<0, Min, Max>, Seed, Body>
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

    struct empty_node
    {
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename H, typename T>
    struct list_node
    {
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

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
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename Item, typename T>
    struct insert<Item, list_node<Item, T>>
    {
        using type = list_node<Item, T>;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename Item, typename H, typename T, bool Less = (hash<Item>::value < hash<H>::value)>
    struct list_insert
    {
        // true case: Item < H
        using type = list_node<Item, list_node<H,T>>;
        using profile_tag = list_insert_tag;
        using profile_types = profile<>;
    };

    template<typename Item, typename H, typename T>
    struct list_insert<Item, H, T, false>
    {
        // true case: Item < H
        using type = list_node<H, typename insert<Item, T>::type>;
        using profile_tag = list_insert_tag;
        using profile_types = profile<insert<Item, T>>;
    };

    template<typename Item, typename H, typename T>
    struct insert<Item, list_node<H, T>>
    {
        using type = typename list_insert<Item, H, T>::type;
        using profile_tag = insert_tag;
        using profile_types = profile<list_insert<Item, H, T>>;
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
            Visitor::template visit<H, T>(std::forward<Args&&>(args)...);
        }
    };

    template<typename T1, typename T2>
    struct merge;

    template<>
    struct merge<empty_node, empty_node>
    {
        using type = empty_node;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename T>
    struct merge<empty_node, T>
    {
        using type = T;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename T>
    struct merge<T, empty_node>
    {
        using type = T;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename H1, typename T1, typename H2, typename T2, bool Less = (hash<H1>::value < hash<H2>::value)>
    struct list_merge
    {
        // true case
        // H1 goes on first
        using type = list_node<H1, typename merge<T1, list_node<H2, T2>>::type>;
        using profile_tag = list_merge_tag;
        using profile_types = profile<merge<T1, list_node<H2, T2>>>;
    };

    template<typename H1, typename T1, typename H2, typename T2>
    struct list_merge<H1, T1, H2, T2, false>
    {
        // false case
        // H2 goes first
        using type = list_node<H2, typename merge<list_node<H1, T1>, T2>::type>;
        using profile_tag = list_merge_tag;
        using profile_types = profile<merge<list_node<H1, T1>, T2>>;
    };

    template<typename H, typename T1, typename T2>
    struct list_merge<H, T1, H, T2, false>
    {
        // Only one H is added to the result
        using type = list_node<H, typename merge<T1, T2>::type>;
        using profile_tag = list_merge_tag;
        using profile_types = profile<merge<T1, T2>>;
    };

    template<typename H1, typename T1, typename H2, typename T2>
    struct merge<list_node<H1, T1>, list_node<H2, T2>>
    {
        using type = typename list_merge<H1, T1, H2, T2>::type;
        using profile_tag = merge_tag;
        using profile_types = profile<list_merge<H1, T1, H2, T2>>;
    };

    template<typename Item, typename List>
    struct contains;

    template<typename Item>
    struct contains<Item, empty_node>
    {
        static const bool value = false;
        using profile_tag = contains_tag;
        using profile_types = profile<>;
    };

    template<typename Item, typename H, typename T, bool Less = (hash<Item>::value < hash<H>::value)>
    struct list_contains
    {
        // False case - go to next item.
        static const bool value = contains<Item, T>::value;
        using profile_tag = list_contains_tag;
        using profile_types = profile<contains<Item, T>>;
    };

    template<typename Item, typename T>
    struct list_contains<Item, Item, T, false>
    {
        static const bool value = true;
        using profile_tag = list_contains_tag;
        using profile_types = profile<>;
    };

    template<typename Item, typename H, typename T>
    struct list_contains<Item, H, T, true>
    {
        // True case: Item not in list
        static const bool value = false;
        using profile_tag = list_contains_tag;
        using profile_types = profile<>;
    };

    template<typename Item, typename H, typename T>
    struct contains<Item, list_node<H, T>>
    {
        static const bool value = list_contains<Item, H, T>::value;
        using profile_tag = list_contains_tag;
        using profile_types = profile<list_contains<Item, H, T>>;
    };

    // Special case just to speed up compilation
    template<typename Item, typename A, typename B, typename T>
    struct contains<Item, list_node<A, list_node<B, T>>>
    {
        static const bool value = type_equals<Item, A>::value || list_contains<Item, B, T>::value; // type_equals<Item, C>::value || contains<Item, T>::value;
        using profile_tag = list_contains_tag;
        using profile_types = profile<list_contains<Item, B, T>>;
    };
    
    // Special case just to speed up compilation
    template<typename Item, typename A, typename B, typename C, typename T>
    struct contains<Item, list_node<A, list_node<B, list_node<C, T>>>>
    {
        static const bool value = type_equals<Item, A>::value || type_equals<Item, B>::value || list_contains<Item, C, T>::value; // type_equals<Item, C>::value || contains<Item, T>::value;
        using profile_tag = list_contains_tag;
        using profile_types = profile<list_contains<Item, C, T>>;
    };

    template<typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<empty_node, Init, Visitor>
    {
        using type = Init;

        using profile_tag = forall_tag;
        using profile_types = profile<Init>;
    };

    template<typename H, typename T, typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<list_node<H, T>, Init, Visitor>
    {
        using T0 = typename Visitor<H, Init>::type;
        using type = typename forall<T, T0, Visitor>::type;

        using profile_tag = forall_tag;
        using profile_types = profile<T0, Visitor<H, Init>, forall<T, T0, Visitor>, Init, type>;
    };

    template<typename List>
    struct make_empty;

    template<>
    struct make_empty<empty_node>
    {
        using type = empty_node;
    };

    template<typename H, typename T>
    struct make_empty<list_node<H, T>>
    {
        using type = empty_node;
    };

    template<template<typename Item> typename Map>
    struct map_body
    {
        template<typename Item, typename List>
        struct body
        {
            using type = typename insert<typename Map<Item>::type, List>::type;
        };
    };


    template<typename List, template<typename Item> typename Map>
    struct map
    {
        using type = typename forall<
            List,
            typename make_empty<List>::type, 
            map_body<Map>::template body>::type;
    };


    template<typename Collection, template<typename Item> typename Predicate>
    struct filter;

    template<template<typename Item> typename Predicate>
    struct filter<empty_node, Predicate>
    {
        using type = empty_node;
    };

    template<typename H, typename T, template<typename Item> typename Predicate, bool Matches = Predicate<H>::value>
    struct filter_list
    {
        // False case
        using type = typename filter<T, Predicate>::type;
    };

    template<typename H, typename T, template<typename Item> typename Predicate>
    struct filter_list<H, T, Predicate, true>
    {
        // True case
        using type = list_node<H, typename filter<T, Predicate>::type>;
    };

    template<typename H, typename T, template<typename Item> typename Predicate>
    struct filter<list_node<H, T>, Predicate>
    {
        using type = typename filter_list<H, T, Predicate>::type;
    };

}