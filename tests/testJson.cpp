#include <simpletest.hpp>

#include <cellar/json.hpp>
#include <cellar/output.hpp>

char example1[] = 
"      {\n"
"        \"Image\": {\n"
"            \"Width\":  800,\n"
"            \"Height\": 600,\n"
"            \"Title\":  \"View from 15th Floor\",\n"
"            \"Thumbnail\": {\n"
"                \"Url\":    \"http://www.example.com/image/481989943\",\n"
"                \"Height\": 125,\n"
"                \"Width\":  100\n"
"            },\n"
"            \"Animated\" : false,\n"
"            \"IDs\": [116, 943, 234, 38793]\n"
"          }\n"
"      }\n";

class JsonTest : public Test::Fixture<JsonTest>
{
public:

    void check(std::string str)
    {
        auto t = cellar::json::parser.parse(str);
        if(!t)
        {
            std::cout << "Syntax error!\n";
        }
        std::cout << t;
        CHECK(t);
    }


    JsonTest()
    {
        AddTest(&JsonTest::Successes);
        name = "Json parsing";
    }

    void Successes()
    {
        check("123");
        check("true");
        check("null");
        check("-12.32");
        check("3.1e43");
        check("[1]");
        check("\"string\"");
        check("[1,2,3,4]");
        check("{ \"Name\": 123 }");
        check("{ \"Name\": 123, \"Age\": 99 }");
        check(example1);
    }
} jt;

void check(std::string str)
{
    auto t = cellar::json::parser.parse(str);
    if(t)
    {
        std::cout << "Parse success!\n";
        std::cout << t;
    }
}

int main()
{
}
