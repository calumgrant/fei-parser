#include <simpletest.hpp>
#include <cellar/json.hpp>
#include <cellar/output.hpp>

int main()
{
    auto t = cellar::json::parser.parse("[\"123\"]");
    if(t)
    {
        std::cout << "Parse success!\n";
        std::cout << t;
    }
}
