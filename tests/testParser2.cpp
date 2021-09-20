#include <cellar/diagnostics.hpp>
#include <simpletest.hpp>

using namespace cellar;

class Diagnostics : public Test::Fixture<Diagnostics>
{
public:
    Diagnostics() : base("Parser diagnostics")
    {
        AddTest(&Diagnostics::Test);
    }

    enum Nodes { Id, Int, Add, Minus, MulNode, DivNode, Brackets, Open, Close };

    using IntToken = token<Int, plus<digit>>;
    using AddToken = token<Add, ch<'+'>>;
    using SubToken = token<Minus, ch<'-'>>;
    using IdToken  = token<Id, plus<alpha>>;
    using OpenToken = token<Open, ch<'('>>;
    using CloseToken = token<Close, ch<')'>>;
    using MulToken = token<MulNode, ch<'*'>>;
    using DivToken = token<DivNode, ch<'/'>>;
    
    struct Expr;
    
    using PrimaryExpr = symbol<
        IntToken,
        IdToken 
        //rule<Brackets, OpenToken, Expr, CloseToken>
        >;
    
    class MulExpr : public symbol<
        PrimaryExpr,
        rule<MulNode, MulExpr, MulToken, PrimaryExpr>
        //rule<DivNode, MulExpr, DivToken, PrimaryExpr>
        > {};

    class Expr : public symbol<
        rule<Add, Expr, AddToken, MulExpr>,
        //rule<Minus, Expr, SubToken, MulExpr>,
        MulExpr
        > {};

    using diags = parser_diagnostics<Expr>;

    struct E : public symbol<
        rule<100, token<0>, token<1>>,
        rule<101, token<0>, E, token<1>>
        > {};

    using diags2 = parser_diagnostics<E>;

    void Test()
    {
        auto parser = make_parser<Expr>();

        std::cout << diags::states() << std::endl; 

//        EQUALS(11, diags2::states().size());  
//        EQUALS(59, diags::states().size()); 

        diags::output_stats();
    }

} diagnostics;

class TemplateCounts : public Test::Fixture<TemplateCounts>
{
public:
    TemplateCounts()
    {
        AddTest(&TemplateCounts::Report);
    }

    void Report()
    {

    }
} tcounts;

#define CREATE_MEMBER_DETECTOR(X)                                                   \
template<typename T> class Detect_##X {                                             \
    struct Fallback { int X; };                                                     \
    struct Derived : T, Fallback { };                                               \
                                                                                    \
    template<typename U, U> struct Check;                                           \
                                                                                    \
    typedef char ArrayOfOne[1];                                                     \
    typedef char ArrayOfTwo[2];                                                     \
                                                                                    \
    template<typename U> static ArrayOfOne & func(Check<int Fallback::*, &U::X> *); \
    template<typename U> static ArrayOfTwo & func(...);                             \
  public:                                                                           \
    typedef Detect_##X type;                                                        \
    enum { value = sizeof(func<Derived>(0)) == 2 };                                 \
};

CREATE_MEMBER_DETECTOR(value);




namespace MaybeEmptyTest
{
    class N1 : public symbol<token<12>> {};

    class N2 : public symbol<N2> {};

    class E3 : public symbol<rule<10>> {};

    static_assert(!impl::maybe_empty<symbol<>, 1>::value, "Maybe empty");
    static_assert(!impl::maybe_empty<N1, 1>::value, "Maybe empty");
    static_assert(impl::maybe_empty<rule<10>, 1>::value, "Maybe empty");
    static_assert(impl::maybe_empty<E3, 2>::value, "Maybe empty");

    static_assert(impl::maybe_empty<N2, 1>::value == false, "Maybe empty");

    class E4 : public symbol<E3, E3> {};
    static_assert(impl::maybe_empty<E4, 10>::value, "Maybe empty");

    class N4 : public symbol<N4, E3, E3> {};
    static_assert(impl::maybe_empty<N4, 10>::value, "Maybe empty");
}

namespace ValidateParsers
{
    // !! Recursion guard needed here
    template<typename Symbol>
    struct is_valid_parser
    {
        static const bool value = false;
    };

    template<typename Item>
    struct is_valid_parser<symbol<Item>>
    {
        static const bool value = is_valid_parser<Item>::value;
    };
}