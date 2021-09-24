#include "grammar.hpp"
#include <cellar/diagnostics.hpp>

struct dump_first
{
    template<typename Item, typename Next>
    static void visit()
    {
        std::cout << "Symbol " << write<Item> << " has first-tokens " << 
            write<typename first<Item>::type> << std::endl;

        visitor<Next>::template visit<dump_first>();
    }
};

int main()
{
    using S = find_all_symbols<::CompilationUnit>::type;
//    static_assert(size<S>::value == 56);
    visitor<S>::visit<dump_first>(); 
}
