#include <cellar/json.hpp>
#include <cstring>

using namespace cellar;

node json::member(node n, const char * key)
{
    if(n.id() == Object)
    {
        int keylen = strlen(key);
        for(auto c : n)
        {
            if(c.id() == Member && strncmp(c[0].c_str()+1, key, keylen)==0)
                return c[1];
        }
    }

    return node();
}

bool json::getValue(node n, int & result)
{
    if(n.id() == json::Number)
    {
        result = atof(n.c_str());
        return true;
    }
    else
    {
        result = 0;
        return false;
    }
}

bool json::getValue(node n, std::string & result)
{
    if(n.id() == json::String)
    {
        result.assign(n.c_str() + 1, n.tokenLength()-2);
        return true;
    }
    else if(n.id() == json::Number)
    {
        result = n.str();
        return true;
    }
    else
    {
        result.clear();
        return false;
    }
}
