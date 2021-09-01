
#include <feiparser.hpp>

namespace Grammar1
{
    using namespace feiparser;
    enum Nodes { Int, Add, Plus, Minus };

    class Expr : public 
        alt<
            rule<Plus, Expr, token<Add>, Expr>,
            token<Int>
            >
    {
    };

    using Tokens = alt<
        token<Int, plus<digit>>,
        token<Add, ch<'+'>>
        >;
}

int main()
{
    auto lexer = feiparser::make_lexer<Grammar1::Tokens>();
    auto parser = feiparser::make_parser<Grammar1::Expr>(lexer);
}
