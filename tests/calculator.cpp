/*
    This is a demo of the Cellar parsing framework.
*/

// This header file includes the entire library.
#include <cellar/cellar.hpp>

#include <iostream>
#include <map>
#include <cmath>

using namespace cellar;

// You need to define identifiers for all tokens and nodes.
// These will be stored in the generated syntax tree.

enum Nodes { ExitNode, IntNode, FloatNode, PlusNode, ExprNode,
    AssignNode, EqNode, IdNode, OpenNode, CloseNode, BracketNode,
    AddNode, SubNode, MinusNode, MulNode, DivNode };

// Define the lexer

// These are the "tokens" in the language.
// When stored in the parse tree, the nodes will have the given identifiers.

// ExitToken has a lower id than IdToken, so it will be matched in favour of IdToken.
using ExitToken = token<ExitNode, string<'e', 'x', 'i', 't'>>;
using Int = plus<digit>;
using IntToken = token<IntNode, Int>;
using EqToken = token<EqNode, ch<'='>>;
using IdToken = token<IdNode, seq<alpha, star<alnum>>>;

using FloatE = optional<seq<chalt<'e','E'>, optional<chalt<'+','-'>>, Int>>;
using Dot = ch<'.'>;

using Float = alt<
    seq<Int, FloatE>,
    seq<Int, Dot, star<digit>, FloatE>,
    seq<Dot, Int, FloatE>
>;

using FloatToken = token<FloatNode, Float>;

using OpenToken = token<OpenNode, ch<'('>>;
using CloseToken = token<CloseNode, ch<')'>>;
using PlusToken = token<PlusNode, ch<'+'>>;
using MinusToken = token<MinusNode, ch<'-'>>;
using MulToken = token<MulNode, ch<'*'>>;
using DivToken = token<DivNode, ch<'/'>>;

// Define the parser

class Expr;

using PrimaryExpr = symbol<
    IntToken,
    FloatToken,
    IdToken,
    rule<BracketNode, OpenToken, Expr, CloseToken>
    >;

using Assignment = rule<AssignNode, IdToken, EqToken, Expr>;

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

using CalculatorGrammar = symbol<ExitToken, Assignment, Expr>;


class Calculator
{
public:
    std::map<std::string, double> variables;
    char_parser parser;

    Calculator() : parser(make_parser<CalculatorGrammar>())
    {
        variables["pi"] = M_PI;
        variables["e"] = M_E;
    }

    bool parse(const std::string & line)
    {
        auto tree = parser.parse(line);
        if(tree.success)
        {
            auto root = tree.root();

            // This is for demo purposes to display the parse tree
            std::cout << "\nHere is the syntax tree:\n" << root << std::endl;

            switch(root.id())
            {
                case ExitNode:
                    return false;
                case AssignNode:
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
            std::cout << "Syntax error at position " << tree.errorLocation.col << std::endl;
        }
        return true;
    }

    double eval(node n)
    {
        switch(n.id())
        {
            case FloatNode:
            case IntNode:
                return atof(n.c_str());
            case IdNode:
                // Keep it nice and simple - undeclared variables are silently initialised to 0.
                return variables[n[0].str()];
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

    std::string line;
    Calculator calc;

    do
    {
        std::cout << "Enter expression> ";
    }
    while(std::getline(std::cin, line) && calc.parse(line));
}