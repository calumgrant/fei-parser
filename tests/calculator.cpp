/*
    This is a demo of the Cellar parsing framework.

    We construct a parser for numerical expressions like 
    `1+2+3` or `1+pi*i`. We can assign variables using `x=...`.

*/

// This header file includes the entire library.
#include <cellar/cellar.hpp>
#include <cellar/diagnostics.hpp>

#include <iostream>
#include <map>
#include <cmath>

// All Cellar names are in this namespace.
using namespace cellar;

// You need to define identifiers for all tokens and nodes.
// These will be stored in the syntax tree.

enum Nodes {
    ExitNode, IntNode, FloatNode, PlusNode, ExprNode,
    AssignNode, EqNode, IdNode, OpenNode, CloseNode, BracketNode,
    AddNode, SubNode, MinusNode, MulNode, DivNode };

// Define the lexer
// Note that the entire grammar is defined using TYPES, not values.
// If you are unfamiliar with C++, then `using` defines a type-alias (much like a `typedef`).

// "Symbols" in the lexer are defined with `using`. They are types, having no run-time overhead.
// The `plus` rule accepts one or more, and `digit` is any digit, defined as `chrange<'0', '9'>`.
// We'll be lax and allow leading zeros.
using Int = plus<digit>;

// `token` defines a token type in the lexer. This is matched as a token in the
// grammar, and the "type"/id() of the node is given by `IntNode`.
using IntToken = token<IntNode, Int>;

// `string` defines a sequence of characters - a keyword.
// Since `IdToken` also matches `ExitToken`, we ensure that the value of 
// `ExitNode` is lower than `IdNode` (it precedes it in the `enum`), so
// it will be matched in favour of `IdNode`.
using ExitToken = token<ExitNode, string<'e', 'x', 'i', 't'>>;

// This is an identifier. `alpha` and `alnum` are a predefined types.
// `seq` defines a sequence of rules, and `star` matches zero or more.
using IdToken = token<IdNode, seq<alpha, star<alnum>>>;

// Now let's define floating point numbers. `optional` matches 0 or 1 times.
// `chalt` matches a character class.
using FloatE = optional<seq<chalt<'e','E'>, optional<chalt<'+','-'>>, Int>>;
using Dot = ch<'.'>;

using Float = alt<
    seq<Int, FloatE>,
    seq<Int, Dot, star<digit>, FloatE>,
    seq<Dot, Int, FloatE>
>;

using FloatToken = token<FloatNode, Float>;

// `ch` matches a single character.
using EqToken = token<EqNode, ch<'='>>;

// Define a few more tokens and we are finished with the lexer.
using OpenToken = token<OpenNode, ch<'('>>;
using CloseToken = token<CloseNode, ch<')'>>;
using PlusToken = token<PlusNode, ch<'+'>>;
using MinusToken = token<MinusNode, ch<'-'>>;
using MulToken = token<MulNode, ch<'*'>>;
using DivToken = token<DivNode, ch<'/'>>;

// Define the parser.
// "Symbols" in the parser are declared either using a `class` or `using`.
// `using` syntax is a bit cleaner, but does not permit recursion.
// Recursion isn't an issue for the lexer because lexer rules cannot be recursive.

// Forward-declare the `Expr` symbol.
class Expr;

// Symbols are defined using the `symbol<>` template, containing the rules in the symbol.
class PrimaryExpr : public symbol<
    IntToken,
    FloatToken,
    IdToken,
    rule<BracketNode, OpenToken, Expr, CloseToken>  // Match "( Expr )"
    > {};

// This "symbol" is just a rule. Remember that `using` is just expanded into whatever
// type uses it.
class Assignment : public symbol<rule<AssignNode, IdToken, EqToken, Expr>> {};

class MulExpr : public symbol <
    PrimaryExpr,
    rule<MulNode, MulExpr, MulToken, PrimaryExpr>,
    rule<DivNode, MulExpr, DivToken, PrimaryExpr>
    > {};

class Expr : public symbol<
    MulExpr,
    rule<AddNode, Expr, PlusToken, MulExpr>,
    rule<SubNode, Expr, MinusToken, MulExpr> 
    > {};

class CalculatorGrammar : public symbol<ExitToken, Assignment, Expr> {};

class Calculator
{
public:
    std::map<std::string, double> variables;

    // This stores the generated parser. Interally, it is just a function-pointer!
    char_parser parser;

    // Construct the parser using make_parser(). The lexer is derived automatically from the grammar, but it's
    // also possible to explicitly use a different lexer.
    // Parser construction is trivial as the work to compute the parsing function is done at compile-time.
    Calculator() : parser(make_parser<CalculatorGrammar>())
    {
        // Pre-populate some useful variables.
        variables["pi"] = M_PI;
        variables["e"] = M_E;

        using Diagnostics = parser_diagnostics<CalculatorGrammar>;

        std::cout << Diagnostics::number_of_states << std::endl << Diagnostics::states();
    }

    // Return `false` to exit.
    bool parse(const std::string & line)
    {
        // Invoke the parser, returning a syntax tree.
        auto tree = parser.parse(line);

        // Check for a successful parse
        if(tree)
        {
            // Parsing was successful

            // Get a reference to the root node.
            auto root = tree.root();

            // Display the parse tree for demo purposes
            std::cout << "\nHere is the syntax tree:\n" << root << std::endl;

            // What is the "id" of the root node? All nodes have an id, specified in the 
            // `rule<>` and `token<>` elements.
            switch(root.id())
            {
                case ExitNode:
                    return false;
                case AssignNode:
                    // Use the subscript operator [] to get a child node.
                    variables[root[0].str()] = eval(root[2]);

                    for(auto i : variables)
                        std::cout << i.first << " = " << i.second << std::endl;
                    break;
                default:
                    std::cout << "Result = " << (variables["result"] = eval(root)) << std::endl;
                    break;
            }
        }
        else
        {
            // Parsing was unsuccessful, and the location of the syntax error is stored in the tree instead.
            std::cout << "Syntax error at position " << tree.errorLocation.col << std::endl;
        }
        return true;
    }

    // This is the function we use to process the parse tree.
    double eval(node n)
    {
        // Determine the type of the node and evaluate it accordingly
        switch(n.id())
        {
            case FloatNode:
            case IntNode:
                // `cstr()` gets the text of a token as a null-terminated C-string.
                // This is efficient as it's already stored in the tree.
                return atof(n.c_str());
            case IdNode:
                // Keep it nice and simple - undeclared variables are silently initialised to 0.
                // `str()` gets a `std::string` of the token text.
                return variables[n.str()];
            case BracketNode:
                return eval(n[1]);

            case AddNode:
                return eval(n[0]) + eval(n[2]);
            case SubNode:
                return eval(n[0]) - eval(n[2]);
            case MulNode:
                return eval(n[0]) * eval(n[2]);
            case DivNode:
                return eval(n[0]) / eval(n[2]);
            default:
                std::cout << "Internal error\n";
                return NAN;
        }

    }
};

int main()
{
    std::cout << "Welcome to the Cellar calculator demo. Type \"exit\" or ^D to exit.\n";

    Calculator calc;
    std::string line;

    do
    {
        std::cout << "Enter expression> ";
    }
    while(std::getline(std::cin, line) && calc.parse(line));
}