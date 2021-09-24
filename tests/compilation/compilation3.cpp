#include "grammar.hpp"
#include <cellar/diagnostics.hpp>

int main()
{
    using S = empty_symbols_in_grammar<::CompilationUnit>::type;
    std::cout << write<S> << std::endl;
    std::cout << "There are " << size<S>::value << " empty symbols in the grammar\n";
}
