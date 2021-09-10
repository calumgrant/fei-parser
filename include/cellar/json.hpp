#pragma once
#include "parser.hpp"

namespace cellar::json
{
    enum Nodes
    {
        False,
        True,
        Null,
        Object,
        Array,
        Number,
        String,
        Member
    };

    char_lexer lexer();
    char_parser parser();

    node member(node object, const char * key);
    bool getValue(node v, std::string & result);
    bool getValue(node v, int & result);
    bool getValue(node v, double & result);

    class Value : public node
    {
    public:
        Value();
        Value(node n);

        Value operator[](int index) const;
        Value operator[](const char * key) const;

        int getInt() const;
        double getDouble() const;
        std::string getString() const;
        bool getBool() const;

        bool isTrue() const;
        bool isFalse() const;
        bool isNumber() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;
    };
}
