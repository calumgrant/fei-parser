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

json::Value::Value(node n) : node(n) {}

json::Value json::Value::operator[](int n) const { return node::operator[](n); }

json::Value json::Value::operator[](const char * key) const
{
    return member(*this, key);
}

int json::Value::getInt() const
{
    int result = 0;
    getValue(*this, result);
    return result;
}

std::string json::Value::getString() const
{
    std::string result;
    getValue(*this, result);
    return result;
}
