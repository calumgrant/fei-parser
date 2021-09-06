# feiparser

Fast, easily-integrated LALR parser.

Feiparser is a parser framework for C++. Unlike many parser-generators, the grammar is specified in C++, so there is no seperate build step.

Features include:

- Extremely fast lexer and parser
- More easily integrated into build systems
- Support for lexical analysers
- A compiled regex library
- Built-in parsers for XML, JSON and Java.
- Generates parse trees

# A simple example

```
#include <feiparser.hpp>

enum Nodes { Int, Add, Sub };

using IntToken = token<Int, plus<chrange<'0','9'>>>;
using AddToken = token<Add, ch<'+'>>;
using SubToken = token<Sub, ch<'-'>>;

class Expr : symbol<
    IntToken,
    rule<Add, Expr, Add, Int>,
    rule<Sub, Expr, Sub, Int>
    > {};

auto parser = make_parser<Expr>();

auto tree = parser.parse("1+2-3");

int eval(node n)
{
    switch(n.id())
    {
        case IntToken: return atoi(n.c_str());
        case Add: return eval(n[0]) + eval(n[2]);
        case Sub: return eval(n[0]) - eval(n[2]));
    }
}

std::cout << eval(tree.root());
```

# Benchmarks

Java
XML
Json
Regexes


I wrote this parser because I was frustrated with how difficult it was to integrate common parser frameworks with C++, and at the same time I was surprised and concerned about how slow they could be. So I set out

# Installation

feiparser is a header-only library, so is installed by copying these header files into your C++ project, installing them on your system, or including them as a submodule in your project.

The build files are only needed to run the tests.

# Defining a tokenizer

A tokenizer (or lexer) splits text up into units called "tokens". Tokens are defined using rules such as "the string `if`" or "any sequence of digits."

In feiparser, rules are defined using structured types.

The 

```
#include <feiparser.hpp>

using namespace cellar;

enum class Tokens
{
    string_token
};

using string_token = token<string_token, seq<ch<'\"'>, notchset<'\"`>, ch<'\"'>>;



```

# Defining a parser

