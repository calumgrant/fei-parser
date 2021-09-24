#include "grammar.hpp"
#include <cellar/diagnostics.hpp>

int main()
{
    using S = empty_symbols_in_grammar2<::CompilationUnit>::type;
    std::cout << write<S> << std::endl;
    std::cout << "There are " << size<S>::value << " potentially empty symbols in the grammar\n";
    static_assert(size<S>::value == 7);
}
