#include "grammar.hpp"
#include <iostream>

char_parser parser2()
{
    return make_parser<::CompilationUnit>(java::lexer());
}

int main()
{
    // Perform a test compilation 

    char input[] = "import foo.bar; /* Comment */ public class Foo ";

    auto tree = parser2().parse(input);

    if(tree)
    {
        std::cout << "Parse success!\n";
        return 0;
    }
    else
    {
        std::cout << "Compilation failed!\n";
        return 1;
    }
}
