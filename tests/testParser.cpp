
#include <feiparser.hpp>

namespace Grammar1
{
    using namespace feiparser;
    enum Nodes { IntNode, AddNode, PlusNode, MinusNode };

    using Int = token<IntNode, plus<digit>>;
    using Add = token<AddNode, ch<'+'>>

    class Expr : public symbol<
            rule<PlusNode, Expr, Add, Expr>,
            Int
            >
        {};

    using Tokens = alt<Int, Add>;
}

int main()
{
    auto parser = feiparser::make_parser<Grammar1::Tokens, Grammar1::Expr>();
}
