#include <cellar/xml.hpp>

// This is suboptimal
#include <cellar/output.hpp>

int main()
{
    auto t = cellar::xml::parser.parse("<ksdjcn>");
    std::cout << t;
}