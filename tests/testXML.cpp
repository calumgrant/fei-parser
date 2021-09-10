#include <cellar/xml.hpp>

// This is suboptimal
#include <cellar/output.hpp>

int main()
{
    auto t = cellar::xml::parser().parse("<?xsml  ?>");

    if(!t)
    std::cout << "Syntax error! at " <<  t.errorLocation.offset << std::endl;

    std::cout << t;
}