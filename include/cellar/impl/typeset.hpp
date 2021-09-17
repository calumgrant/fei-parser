#pragma once

#include "hash.hpp"

namespace cellar
{
    template<typename...Members>
    struct typeset;

    template<typename Item, typename Ts>
    struct typeset_contains;

    template<typename Item, typename Ts>
    struct typeset_insert;

    template<typename...Members>
    struct typeset
    {
        constexpr int size() { return sizeof...(Members); }

        constexpr bool empty() { return size()==0; }

        template<typename Item>
        constexpr bool contains(Item)
        {
            return typeset_contains<Item, typeset>::value;
        }

        template<typename Item>
        constexpr auto insert(Item)
        {
            return typeset_insert<Item, typeset>::type();
        }

        using profile_tag = typeset_tag;
        using profile_types = profile<Members...>;
    };

    template<typename Set, typename Item>
    struct typeset_insert;

    template<typename Item>
    struct typeset_insert<Item, typeset<>>
    {
        typedef typeset<Item> type;
        using profile_tag = typeset_insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename...Members>
    struct typeset_insert<Item, typeset<Item, Members...>>
    {
        typedef typeset<Item, Members...> type;

        using profile_tag = typeset_insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename TypeSet> struct typeset_ins {};

    template<typename Item, typename... Members>
    struct typeset_ins<Item, typeset<Members...>>
    {
        typedef typeset<Item, Members...> type;
        using profile_tag = typeset_insert_tag;
        using profile_types = profile<type>;
    };

    template<typename Item, typename Item2, typename...Members>
    struct typeset_insert<Item, typeset<Item2, Members...>>
    {
        using S = typename typeset_insert<Item, typeset<Members...>>::type;
        typedef typename typeset_ins<Item2, S>::type type;

        using profile_tag = typeset_insert_tag;
        using profile_types = profile<typeset_insert<Item, typeset<Members...>>, typeset_ins<Item2, S>, type>;
    };

    template<typename T1, typename T2>
    struct typeset_union;

    template<typename T2>
    struct typeset_union<typeset<>, T2>
    {
        typedef T2 type;
    };

    template<typename Item, typename...Members, typename T2>
    struct typeset_union<typeset<Item, Members...>, T2>
    {
        using S = typename typeset_union<typeset<Members...>, T2>::type;
        using type = typename typeset_insert<Item, S>::type;

        using profile_tag = typeset_union_tag;
        using profile_types = profile<type, typeset_union<typeset<Members...>, T2>, typeset_insert<Item, S>>;
    };

    template<typename Ts>
    struct typeset_size;

    template<>
    struct typeset_size<typeset<>>
    {
        static const int value = 0;
        using profile_tag = typeset_size_tag;
        using profile_types = profile<typeset<>>;
    };

    template<typename Item, typename... Members>
    struct typeset_size<typeset<Item, Members...>>
    {
        static const int value = 1 + typeset_size<typeset<Members...>>::value;
        using profile_tag = typeset_size_tag;
        using profile_types = profile<typeset_size<typeset<Members...>>>;
    };

    template<typename Item, typename Ts>
    struct typeset_contains;

    template<typename Item>
    struct typeset_contains<Item, typeset<>>
    {
        static const bool value = false;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<typeset<>>;
    };

    template<typename Item>
    struct typeset_contains<Item, typeset<Item>>
    {
        static const bool value = true;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<Item>>;
    };

    template<typename Item, typename I0>
    struct typeset_contains<Item, typeset<I0>>
    {
        static const bool value = false;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<I0>>;
    };

    template<typename Item, typename I0, typename I1, typename... Items>
    struct typeset_contains<Item, typeset<I0, I1, Items...>>
    {
        static const bool value = typeset_contains<Item, typeset<Items...>>::value;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<I0, I1, Items...>, typeset_contains<Item, typeset<Items...>>>;
    };

    template<typename Item, typename I1, typename... Items>
    struct typeset_contains<Item, typeset<Item, I1, Items...>>
    {
        static const bool value = true;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<Item, I1, Items...>>;
    };

    template<typename Item, typename I0, typename... Items>
    struct typeset_contains<Item, typeset<I0, Item, Items...>>
    {
        static const bool value = true;
        using profile_tag = typeset_contains_tag;
        using profile_types = profile<Item, typeset<I0, Item, Items...>>;
    };

/*
    template<typename Item, typename I0, typename I1, typename I2, typename...Items>
    struct typeset_contains<Item, typeset<I0, I1, I2>>
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
        static const bool value = typeset_contains<Item, Ts2>::value &&
            typeset_subset<typeset<Items...>, Ts2>::value;

        using profile_tag = typeset_subset_tag;
        using profile_types = profile<
            typeset<Item, Items...>,
            Ts2,
            typeset_contains<Item, Ts2>,
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
        return typeset_union<typeset<Ts1...>, typeset<Ts2...>>::type();
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
    struct tree_size
    {
        // Default case - the tree is a single item.
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
        using profile_tag = no_tag;
        using profile_types = profile<mixed_loop<Init, Mid, Mid+1, Body>, mixed_loop<T1, N1, Mid, Body>, mixed_loop<T0, Mid+1, N2, Body>>;
    };

    template<typename Init, int N, template<typename Value, int Item> typename Body>
    struct mixed_loop<Init, N, N, Body>
    {
        using type = Init;
        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename Init, int N, template<typename Value, int Item> typename Body>
    struct mixed_loop<Init, N, N+1, Body>
    {
        using type = typename Body<Init, N>::type;
        using profile_tag = no_tag;
        using profile_types = profile<Body<Init, N>>;
    };

    template<typename Tree, typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall
    {
        // Default tree of one item
        using type = typename Visitor<Tree, Init>::type;
    };

    template<typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<empty_tree, Init, Visitor>
    {
        using type = Init;
    };

    template<typename H, typename L, typename R, typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<type_tree<H, L, R>, Init, Visitor>
    {
        using type = typename forall<R, typename Visitor<H, typename forall<L, Init, Visitor>::type>::type, Visitor>::type;
    };

    template<typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<typeset<>, Init, Visitor>
    {
        using type = Init;

        using profile_tag = no_tag;
        using profile_types = profile<>;
    };

    template<typename I, typename...Is, typename Init, template<typename Item, typename Aggregate> typename Visitor>
    struct forall<typeset<I, Is...>, Init, Visitor>
    {
        using T = typename Visitor<I, Init>::type;
        using type = typename forall<typeset<Is...>, T, Visitor>::type;

        using profile_tag = no_tag;
        using profile_types = profile<Visitor<I, Init>, forall<typeset<Is...>, T, Visitor>>;
    };

    // Extracts an element from a tree
    template<typename T, int Element>
    struct tree_element
    {
        // Case where the tree consists of one element
        using type = T;
    };

    // Extracts an element from a type_tree.
    // There are three cases:
    //   - element is in the left (`InLeft=true`),
    //   - element is in the right (`InRight=true`) or
    //   - element is at the root
    template<typename H, typename L, typename R, int Element, 
        bool InLeft = (Element < tree_size<L>::value), bool InRight = (Element > tree_size<L>::value)>
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
        using profile_tag = tree_tag;
        using profile_types = profile<Item, T, tree_insert2<Item, T, empty_tree, empty_tree>>;
    };

    template<typename Item>
    struct tree_insert<Item, Item>
    {
        using type = Item;
        using profile_tag = tree_tag;
        using profile_types = profile<Item>;
    };


    template<typename Item>
    struct tree_insert<Item, empty_tree>
    {
        using type = Item;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, empty_tree>;
    };

    template<typename Item, typename H, typename L, typename R, bool Less>
    struct tree_insert2
    {
        // False case - insert to the right
        using type = type_tree<H, L, typename tree_insert<Item, R>::type>;
        using profile_tag = tree_tag;
        using profile_types = profile<tree_insert<Item, R>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct tree_insert2<Item, H, L, R, true>
    {
        // True case - insert to the left
        using type = type_tree<H, typename tree_insert<Item, L>::type, R>;
        using profile_tag = tree_tag;
        using profile_types = profile<tree_insert<Item, L>>;
    };

    template<typename Item, typename L, typename R>
    struct tree_insert<Item, type_tree<Item,L,R>>
    {
        using type = type_tree<Item,L,R>;
        using profile_tag = tree_tag;
        using profile_types = profile<type_tree<Item,L,R>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct tree_insert<Item, type_tree<H,L,R>>
    {
        using type = typename tree_insert2<Item, H, L, R>::type;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, type_tree<H,L,R>, tree_insert2<Item, H, L, R>>;
    };

    template<typename Item, typename T>
    struct tree_contains
    {
        static const bool value = false;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, T>;
    };

    template<typename Item>
    struct tree_contains<Item, empty_tree>
    {
        static const bool value = false;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, empty_tree>;
    };

    template<typename Item>
    struct tree_contains<Item, Item>
    {
        static const bool value = true;
        using profile_tag = tree_tag;
        using profile_types = profile<Item>;
    };

    template<typename Item, typename L, typename R>
    struct tree_contains<Item, type_tree<Item, L, R>>
    {
        static const bool value = true;
        using profile_tag = tree_tag;
        using profile_types = profile<Item, L, R>;
    };

    template<typename Item, typename H, typename L, typename R, bool Less = (hash<Item>::value < hash<H>::value)>
    struct tree_contains2
    {
        // true case: Search left subtree
        static const bool value = tree_contains<Item, L>::value;
        using profile_tag = tree_tag;
        using profile_types = profile<tree_contains<Item, L>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct tree_contains2<Item, H, L, R, false>
    {
        // false case: Search right subtree
        static const bool value = tree_contains<Item, R>::value;
        using profile_tag = tree_tag;
        using profile_types = profile<tree_contains<Item, R>>;
    };

    template<typename Item, typename H, typename L, typename R>
    struct tree_contains<Item, type_tree<H, L, R>>
    {
        static const bool value = tree_contains2<Item, H, L, R>::value;
        using profile_tag = tree_tag;
        using profile_types = profile<H, Item, L, R, tree_contains2<Item, H, L, R>>;
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


    // Maybe delete
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
        struct forall
        {
            using type = typename cellar::forall<Tree, Start, Visitor>::type;
        };

        template<int Element>
        struct element
        {
            using type = typename tree_element<Tree, Element>::type;
        };
    };

    using empty_typeset2 = typeset2<empty_tree>;
}
