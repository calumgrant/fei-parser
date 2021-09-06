
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

namespace TestFollow
{
    using G0 = rule_position<rule<1, token<0>, token<1>>, 0, 2>;
    using G1 = rule_position<rule<1, token<0>, token<1>>, 1, 2>;

    using F0 = follow<G0>::type;
    using F1 = follow<G1>::type;

    static_assert(typeset_equals<F0, typeset<token<1>>>::value, "");
    static_assert(typeset_equals<F1, typeset<token<2>>>::value, "");

    using maybe0 = symbol< rule<1>, token<0> >;
    using maybe1 = symbol< rule<2>, token<1> >;
    using R3 = rule<3, maybe0, maybe1>;
    using S3a = rule_position<R3, 0, 2>;
    using S3b = rule_position<R3, 1, 2>;

    static_assert(typeset_equals<follow<S3a>::type, typeset<token<1>, token<2>>>::value, "");
    static_assert(typeset_equals<follow<S3b>::type, typeset<token<2>>>::value, "");
}

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

namespace Grammar2
{
    using a = token<0>;
    using b = token<1>;

    class E : public symbol<rule<10, a, b>, rule<11, a, E, b>> {};

    using S0 = typeset<rule_position<rule<12, E, token<EndOfStream>>, 0, EndOfStream>>;

    using C0 = closure<S0>::type;

    using A1_0 = action<S0, 0>::type;
    using A1_1 = action<S0, 1>::type;

    using simpletest = typeset<rule_position<rule<10, a, b>, 0, -4>>;
    using Sa = action<simpletest, 0>::type;
    using Sb = action<simpletest, 1>::type;

    using A2a = shift_action<S0, 0>::type;
    using A2b = shift_action<S0, 1>::type;
    using C2 = closure<A2a>::type;
}

int main()
{
    auto parser = feiparser::make_parser<Grammar1::Tokens, Grammar1::Expr>();

    std::cout << TestClosure::S0() << std::endl;
    std::cout << TestClosure::S1() << std::endl;
    std::cout << TestClosure::S2() << std::endl;
    std::cout << TestClosure::Gclosure() << std::endl;

    parser.parse("1+1");

    std::cout << "S0: " << Grammar2::C0() << std::endl;
    std::cout << "action(S0, 0): " << Grammar2::A1_0() << std::endl;
    std::cout << Grammar2::Sa() << Grammar2::Sb() << std::endl;
    std::cout << Grammar2::A2a() << Grammar2::A2b() << Grammar2::C2() << std::endl;
}
