
#include <feiparser.hpp>

namespace Grammar1
{
    using namespace feiparser;
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


int main()
{
    auto parser = feiparser::make_parser<Grammar1::Tokens, Grammar1::Expr>();
}
