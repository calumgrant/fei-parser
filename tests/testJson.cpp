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
        auto t = cellar::json::parser().parse(str);
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
        AddTest(&JsonTest::TestAPI);
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

    void TestAPI()
    {
        auto t = cellar::json::parser().parse(example1);

        // Find Image.Width
        auto image = cellar::json::member(t.root(), "Image");
        CHECK(image);
        auto ids = cellar::json::member(image, "IDs");
        CHECK(ids);
        int i;
        CHECK(cellar::json::getValue(ids[2], i));
        EQUALS(234, i);

        CHECK(cellar::json::getValue(ids[0], i));
        EQUALS(116, i);

        CHECK(cellar::json::getValue(ids[3], i));
        EQUALS(38793, i);

        auto title = cellar::json::member(image, "Title");
        CHECK(title);

        std::string str;
        CHECK(cellar::json::getValue(title, str));
        EQUALS(str, "View from 15th Floor");
    }
} jt;

int main()
{
}
