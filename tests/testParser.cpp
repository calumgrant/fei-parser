
#include <feiparser.hpp>
#include "output.hpp"
    
using namespace feiparser;

namespace Grammar1
{
    enum Nodes { IntNode, AddNode, PlusNode, MinusNode };

    using Int = token<IntNode, plus<digit>>;
    using Add = token<AddNode, ch<'+'>>;

    class Expr : public symbol<
            rule<PlusNode, Expr, Add, Expr>,
            Int
            >
        {};

    class Expr1 : public symbol<
            rule<PlusNode, Int, Add, Int>,
            Int
            >
        {};


    using Tokens = alt<Int, Add>;

    static_assert(potentially_empty_symbol<rule<123>>::value, "");
    static_assert(!potentially_empty_symbol<rule<123, Int>>::value, "");
    static_assert(!potentially_empty_symbol<Int>::value, "");
    static_assert(!potentially_empty_symbol<Expr1>::value, "");
    static_assert(!potentially_empty_symbol<Expr>::value, "");

    class E1 : public symbol<rule<123>> {};
    class E2 : public symbol<E1> {};
    class E3 : public symbol<rule<10, E1>> {};
    class E4 : public symbol<rule<10, E3, E3>> {};
    class E5 : public symbol<Int, E4> {};
    class E6 : public symbol<E5, Int> {};

    static_assert(potentially_empty_symbol<E1>::value, "");
    static_assert(potentially_empty_symbol<E2>::value, "");

    // TODO: Recursive case

    class E7 : public symbol<E7, E1> {};
    static_assert(potentially_empty_symbol<E7>::value, "");

    using E8 = symbol<E5, Int>;
    static_assert(potentially_empty_symbol<E8>::value, "");

    class E9 : public symbol<E9, E9, E1> {};
    static_assert(potentially_empty_symbol<E9>::value, "");
}

namespace FirstTests
{
    using None = typeset<>;
    using None = first<symbol<>>::type;
    using z = typeset<token<0>>;

    using z = first<symbol<token<0>>>::type;
    using z = first<symbol<token<0>, token<0>>>::type;
    using z2 = typeset<token<1>, token<0>>;
    using z2 = first<symbol<token<0>, token<1>>>::type;

    class Empty1 : public rule<0> {};

    class S : public symbol<
        rule<0>, 
        rule<1, token<0>>,
        rule<2, Empty1, Empty1, token<1>>,
        rule<2, Empty1, S, token<2>>,
        rule<3, token<0>, token<9>>
        > {};

    using firstS = first<S>::type;

    static_assert(typeset_contains<token<0>, firstS>::value, "");
    static_assert(typeset_contains<token<1>, firstS>::value, "");
    static_assert(typeset_contains<token<2>, firstS>::value, "");
    static_assert(!typeset_contains<token<9>, firstS>::value, "");
};

namespace TestClosure
{
    using A = token<0>;
    using B = token<1>;

    class Expr : public symbol<rule<10,A>, rule<11, Expr, A>> {};

    using S0 = typeset<rule_position<A, 0, 1>, rule_position<rule<123, A, B>,0,2>>;
    using S1 = typeset<rule_position<A, 0, 1>, rule_position<rule<123, A, B>,0,2>, rule_position<rule<123, A, B>,2,2>>;
    using S2 = typeset<rule_position<rule<123, Expr>, 0, 1>>;

    using Gclosure = closure<S2>::type;
}

int main()
{
    auto parser = feiparser::make_parser<Grammar1::Tokens, Grammar1::Expr>();

    std::cout << TestClosure::S0() << std::endl;
    std::cout << TestClosure::S1() << std::endl;
    std::cout << TestClosure::S2() << std::endl;
    std::cout << TestClosure::Gclosure() << std::endl;

    parser.parse("1+1");
}
