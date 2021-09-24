#include "grammar.hpp"
#include <cellar/diagnostics.hpp>

int main()
{
    using S = find_all_symbols<::CompilationUnit>::type; 
    std::cout << write<S> << std::endl;
    std::cout << "There are " << size<S>::value << " symbols in the grammar\n";
//    static_assert(size<S>::value == 56); 
}
