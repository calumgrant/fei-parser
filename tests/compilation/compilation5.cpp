#include "grammar.hpp"
#include <cellar/diagnostics.hpp>

int main()
{
#if 0
    using Diagnostics = parser_diagnostics<::CompilationUnit>;

    std::cout << "Here are the states:\n" << write<Diagnostics::states> << std::endl;

    std::cout << "There are " << Diagnostics::number_of_states << " states in the state machine " << std::endl;   
#endif
    // std::cout << "Here are the template instantiation stats:\n";
    // Diagnostics::output_stats(); 


    using S = parser_diagnostics<::CompilationUnit>::states;

    std::cout << write<S> << std::endl;
    std::cout << "There are " << size<S>::value << " states in the parser\n";
//    static_assert(size<S>::value == 56);
}
