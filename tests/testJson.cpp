#include <simpletest.hpp>

#include <cellar/json.hpp>
#include <cellar/output.hpp>
#include <sstream>

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
        AddTest(&JsonTest::TestValueAPI);
#if NDEBUG
        AddTest(&JsonTest::TestLexerPerformance);
        AddTest(&JsonTest::TestParserPerformance);
#endif
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
        using namespace cellar::json;

        auto t = parser().parse(example1);

        // Find Image.Width
        auto image = member(t.root(), "Image");
        CHECK(image);
        auto ids = member(image, "IDs");
        CHECK(ids);
        int i;
        CHECK(getValue(ids[2], i));
        EQUALS(234, i);

        CHECK(getValue(ids[0], i));
        EQUALS(116, i);

        CHECK(getValue(ids[3], i));
        EQUALS(38793, i);

        auto title = member(image, "Title");
        CHECK(title);

        std::string str;
        CHECK(cellar::json::getValue(title, str));
        EQUALS(str, "View from 15th Floor");
    }

    void TestValueAPI()
    {
        using namespace cellar::json;

        auto t = parser().parse(example1);

        // Find Image.Width
        Value r = t.root();

        auto image = r["Image"];
        CHECK(image);
        auto ids = image["IDs"];
        CHECK(ids);

        EQUALS(234, ids[2].getInt());
        EQUALS(116, ids[0].getInt());
        EQUALS(38793, ids[3].getInt());

        auto title = image["Title"];
        CHECK(title);

        EQUALS("View from 15th Floor", title.getString());
    }

    std::string MakeTest()
    {
        int size = 10000000;

        std::stringstream contents;
        contents << "[0";
        for(int i=1; i<size; ++i)
            contents << "," << i;
        contents << "]";

        return std::move(contents.str());
    }

    void TestLexerPerformance()
    {
        using namespace cellar::json;
        auto contents = MakeTest();

        auto tokens = lexer().tokenize(contents.data(), contents.data() + contents.size());
        int count=0;
        while(tokens.lex())
            ++count;
        std::cout << "Input size = " << contents.size() << std::endl;
        std::cout << "There are " << count << " tokens\n";
        EQUALS(20000001, count);
    }


    void TestParserPerformance()
    {
        using namespace cellar::json;
        auto contents = MakeTest();

        auto t = parser().parse(contents);
    }
} jt;

int main()
{
}
