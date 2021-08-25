#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <exception>
#include <set>

int parseHex(char c)
{
    if(c>='0' && c<='9') return c-'0';
    if(c>='A' && c<='F') return c-'A'+10;
    throw std::runtime_error("Invalid hex value");
}

struct UnicodeLine
{
    int charValue;
    std::string name;
    char type[2];

    std::string typeString() const
    {
        std::string s;
        s += type[0];
        s+= type[1];
        return s;
    }

    UnicodeLine(const std::string & src)
    {
        charValue = 0;
        const char * p;
        for(p = src.data(); *p != ';'; ++p)
        {
            charValue = charValue * 16 + parseHex(*p);
        }

        for(++p; *p != ';'; ++p)
        {
            name += *p;
        }
        ++p;
        type[0] = p[0];
        type[1] = p[1];
        p+=3;
    }
};

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: gen <Datafile.txt>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    std::string line;
    int count=0;

    std::set<std::string> types;

    while(std::getline(file, line))
    {
        UnicodeLine ul(line);
        std::cout << ul.charValue << " " << ul.name << " " << ul.type[0] << ul.type[1] << std::endl;
        types.insert(ul.typeString());
        ++count;
    }
    std::cout << "There are " << types.size() << " types\n";
    std::cout << "There are " << count << " characters\n";
}