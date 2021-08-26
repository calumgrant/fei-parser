# feiparser

Fast, easily-integrated parser.

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

using namespace feiparser;

enum class Tokens
{
    string_token
};

using string_token = token<string_token, seq<ch<'\"'>, notchset<'\"`>, ch<'\"'>>;



```

# Defining a parser

