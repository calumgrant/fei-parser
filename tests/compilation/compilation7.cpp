
#define MIN_GRAMMAR 1
#include "grammar.hpp"
#include <cellar/diagnostics.hpp>


int main()
{
    using D = parser_diagnostics<::CompilationUnit>;
    using S = D::states;

    std::cout << "There are " << size<S>::value << " states in the parser\n";
    std::cout << write<D>;
}
