
#include <cellar/cellar.hpp>
#include <cellar/output.hpp>
#include <simpletest.hpp>
    
using namespace cellar;

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

namespace Conflicts1
{
    using a = token<0>;
    using b = token<1>;

    using Expr = symbol<rule<1, a>, rule<2, a>>;

    using S0 = initial_state<Expr>::type;
    using S1 = shift_action<S0, 0>::type;
}

template<typename State, int Token>
void outputAction()
{
    std::cout << "Action on " << Token << " = " << typename action<State, Token>::actions() << " -> " << typename shift_action<State,Token>::type() << std::endl;
}

template<typename State>
void outputActions(typeset<>) {}


template<typename State, int Token, typename... T>
void outputActions(typeset<token<Token>, T...>)
{
    outputAction<State, Token>();
    outputActions<State>(typeset<T...>());
}

template<typename State, typename Symbol>
void outputGoto()
{
    std::cout << "GOTO " << Symbol() << " -> " << typename goto_<State, Symbol>::type() << std::endl;
}

template<typename State>
void outputGotos(typeset<>)
{
}

template<typename State, typename Sym, typename...Syms>
void outputGotos(typeset<Sym, Syms...>)
{
    outputGoto<State, Sym>();
}

template<typename State>
void outputState()
{
    using C = typename closure<State>::type;
    std::cout << "State kernel: " << State();
    std::cout << "Closure: " << C() << std::endl;;

    // Show the transitions
    using Tokens = typename build_next_token_list<C>::type;
    outputActions<State>(Tokens());

    using Gotos = typename build_goto_list<C>::type;
    outputGotos<State>(Gotos());
}



class FindTokensInGrammar : public Test::Fixture<FindTokensInGrammar>
{
    enum Nodes { IntNode, AddNode, PlusNode, MinusNode };

    using Int = token<IntNode, plus<digit>>;
    using Add = token<AddNode, ch<'+'>>;

    class Expr : public symbol<
            rule<PlusNode, Expr, Add, Expr>,
            Int
            >
        {};

    using S0 = initial_state<Expr>::type;
    using C0 = closure<S0>::type;
public:

    FindTokensInGrammar()
    {
        name = "Finding tokens in a grammar - todo";
        AddTest(&FindTokensInGrammar::f);
    }

    void f()
    {     
        outputState<S0>();
    }
} find_tokens;


class Closure1 : public Test::Fixture<Closure1>
{
    using A = token<0>;
    using B = token<1>;

    class Expr : public symbol<rule<10,A>, rule<11, Expr, A>> {};

    using S0 = typeset<rule_position<A, 0, 1>, rule_position<rule<123, A, B>,0,2>>;
    using S1 = typeset<rule_position<A, 0, 1>, rule_position<rule<123, A, B>,0,2>, rule_position<rule<123, A, B>,2,2>>;
    using S2 = typeset<rule_position<rule<123, Expr>, 0, 1>>;

    using Gclosure = closure<S2>::type;

public:
    Closure1() { 
        name = "Closures";
        AddTest(&Closure1::Output); 
    }

    void Output()
    {
        std::cout << S0() << std::endl;
        std::cout << S1() << std::endl;
        std::cout << S2() << std::endl;
        std::cout << Gclosure() << std::endl;
    }
} c1;

class Grammar2 : public Test::Fixture<Grammar2>
{
    using a = token<0>;
    using b = token<1>;

    class E : public symbol<rule<10, a, b>, rule<11, a, E, b>> {};

    using S0 = typeset<rule_position<rule<12, E, token<EndOfStream>>, 0, EndOfStream>>;

    using C0 = closure<S0>::type;

    using A0a = action<S0, 0>::shift_actions;
    using A0b = action<S0, 1>::shift_actions;
    using Error = typeset<>;

    using simpletest = typeset<rule_position<rule<10, a, b>, 0, -4>>;
    using Sa = action<simpletest, 0>::actions;
    using Error1 = action<simpletest, 1>::actions;
    static_assert(Error() == Error1(), "");

    using S1 = shift_action<S0, 0>::type;
    using Error2 = shift_action<S0, 1>::type;
    static_assert(Error() == Error2(), "");
    using C1 = closure<S1>::type;

    using S2 = goto_<S0, E>::type;

    using S3 = shift_action<S1, 0>::type;
    using C3 = closure<S3>::type;

    using S4 = shift_action<S3, 1>::type;
    using C4 = closure<S4>::type;

    using S5 = shift_action<S1, 1>::type;
    using S3a = shift_action<S3, 0>::type;
    static_assert(S3() == S3a(), "");

    using C5 = closure<S5>::type;
    using S6 = goto_<S1, E>::type;
    using S7 = goto_<S3, E>::type;
    using S8 = shift_action<S6, 1>::type;
    using S9 = shift_action<S7, 1>::type;

    using Empty = goto_<S2, E>::type;  // Just checking it's still empty    
public:
    Grammar2()
    {
        AddTest(&Grammar2::Test);
    }

    void Test()
    {
        std::cout << "C0: " << Grammar2::C0() << std::endl;
        std::cout << "action(S0, 0): " << Grammar2::A0a() << std::endl;
        std::cout << Grammar2::Sa() << std::endl;
        std::cout << "C1: " << Grammar2::C1() << std::endl;
        std::cout << "C3: " << Grammar2::C3() << std::endl;
        std::cout << "C4: " << Grammar2::C4() << std::endl;
        std::cout << "C5: " << Grammar2::C5() << std::endl;
        outputState<Grammar2::S0>();
        outputState<Grammar2::S1>();
        outputState<Grammar2::S3>();
        outputState<Grammar2::S4>();
        outputGoto<Grammar2::S0, Grammar2::E>();
    }
} g2;


int main()
{

    // Let's try a conflict
    outputState<Conflicts1::S0>();
    outputState<Conflicts1::S1>();
}


class Parse1 : public Test::Fixture<Parse1>
{
public:
    Parse1()
    {
        AddTest(&Parse1::TestParse);
        AddTest(&Parse1::TestTokenize);
    }

    enum Nodes { IntNode, AddNode, PlusNode, MinusNode };

    using Int = token<IntNode, plus<digit>>;
    using Add = token<AddNode, ch<'+'>>;

    class Expr : public symbol<
            rule<PlusNode, Expr, Add, Int>,
            Int
            >
        {};

    using Tokens = alt<Int, Add>;
    
    void TestTokenize()
    {
        auto l = make_lexer<Tokens>();
        auto tokens = l.tokenize("1+1");
        CHECK(tokens.lex());
        EQUALS(IntNode, tokens.token());
        CHECK(tokens.lex());
        EQUALS(AddNode, tokens.token());
        CHECK(tokens.lex());
        EQUALS(IntNode, tokens.token());
        CHECK(!tokens.lex());
        EQUALS(EndOfStream, tokens.token());
    }

    void TestParse()
    {
        auto parser = cellar::make_parser<Tokens, Expr>();

        auto t1 = parser.parse("12");
        CHECK(t1.success);
        t1 = parser.parse("1+1");
        CHECK(t1.success);
    }
} p1;
