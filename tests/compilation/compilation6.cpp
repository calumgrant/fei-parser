#include "grammar.hpp"
#include <cellar/diagnostics.hpp>



int main()
{

    using S = parser_diagnostics<::CompilationUnit>::states;
    using C = map<S, closure>::type;

    std::cout << write<C> << std::endl;
    std::cout << "There are " << size<C>::value << " states in the parser\n";
//    static_assert(size<S>::value == 56);
}
