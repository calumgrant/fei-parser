#pragma once

#include "cellar.hpp"

#include <iostream>
#include <cctype>

namespace cellar
{
    template<typename... Items>
    struct list {};

    inline std::ostream & operator<<(std::ostream & os, list<>) { return os; }

    template<typename Item>
    std::ostream & operator<<(std::ostream & os, list<Item>) { return os << Item(); }

    template<int Ch>
    std::ostream & operator<<(std::ostream & os, ch<Ch>)
    {
        return os << "ch<'" << char(Ch) << "'>";
    }

    template<int Ch>
    std::ostream & operator<<(std::ostream & os, notch<Ch>)
    {
        return os << "notch<'" << char(Ch) << "'>";
    }

    template<int Ch, int C2, int... Chs>
    std::ostream & operator<<(std::ostream & os, notch<Ch, C2, Chs...>)
    {
        return os << "notch<'" << char(Ch) << "," << notch<C2, Chs...>() << "'>";
    }

    template<int Ch>
    std::ostream & operator<<(std::ostream & os, string<Ch>)
    {
        return os << "ch<'" << char(Ch) << "'>";
    }

    template<int Ch1, int Ch2, int...Chs>
    std::ostream & operator<<(std::ostream & os, string<Ch1, Ch2, Chs...>)
    {
        return os << "seq<char<'" << char(Ch1) << "'>," << string<Ch2, Chs...>() << ">";;
    }

    template<typename...Rules>
    std::ostream & operator<<(std::ostream & os, seq<Rules...>)
    {
        return os << "seq<" << list<Rules...>() << '>';
    }

    template<typename Rule>
    std::ostream & operator<<(std::ostream & os, optional<Rule>)
    {
        return os << "optional<" << Rule() << '>';
    }

    template<int...Chs>
    std::ostream & operator<<(std::ostream & os, chalt<Chs...>)
    {
        return os << "chalt<...>";
    }


    template<typename...Rules>
    std::ostream & operator<<(std::ostream & os, alt<Rules...>)
    {
        return os << "alt<" << list<Rules...>() << '>';
    }

    inline std::ostream & operator<<(std::ostream & os, empty)
    {
        return os << "empty";
    }

    inline std::ostream & operator<<(std::ostream & os, reject)
    {
        return os << "reject";
    }

    template<typename R>
    std::ostream & operator<<(std::ostream & os, star<R>)
    {
        return os << "star<" << R() << ">";
    }

    template<typename R>
    std::ostream & operator<<(std::ostream & os, plus<R>)
    {
        return os << "plus<" << R() << ">";
    }

    template<int Ch1, int Ch2>
    std::ostream & operator<<(std::ostream & os, chrange<Ch1, Ch2>)
    {
        return os << "chrange<'" << (char)Ch1 << "','" << (char)Ch2 << "'>";
    }


    template<typename Item1, typename Item2, typename...Items>
    std::ostream & operator<<(std::ostream & os, list<Item1, Item2, Items...>) 
    {
        return os << Item1() << ", " << list<Item2,Items...>();
    }


    template<int Token, typename Rule, typename...Rules>
    std::ostream & operator<<(std::ostream & os, token<Token,Rule,Rules...>)
    {
        return os << "token<" << Token << "," << list<Rule,Rules...>() << ">";
    }

    template<int Token>
    std::ostream & operator<<(std::ostream & os, token<Token>)
    {
        if(Token == EndOfStream)
            return os << "$";
        
        return os << "token<" << Token << ">";
    }


    template<typename It>
    std::ostream & operator<<(std::ostream & os, const token_stream<It> & ts)
    {
        os << "[";
        for(auto i : ts)
            os << i;
        os << "]=" << ts.token();
        return os;
    }

    inline void output_typeset(std::ostream & os, const typeset<> & ts) {}

    template<typename Item>
    void output_typeset(std::ostream & os, const typeset<Item> &)
    {
        os << Item();
    }

    template<typename Item1, typename Item2, typename...Items>
    void output_typeset(std::ostream & os, const typeset<Item1, Item2, Items...> & ts)
    {
        os << Item1() << ",\n  ";
        output_typeset(os, typeset<Item2, Items...>());
    }

    template<typename...Rules>
    std::ostream & operator<<(std::ostream & os, const typeset<Rules...> & ts)
    {
        os << "{\n  ";
        output_typeset(os, ts);
        return os << "\n}";
    }

    template<int Position, typename...Symbols>
    struct write_rule;

    inline void writeSymbol(std::ostream & os, WholeProgram)
    {
        os << "WholeProgram";
    }

    template<int Id, typename...Items>
    inline void writeSymbol(std::ostream & os, rule<Id, Items...>)
    {
        os << "rule<" << Id << ">";
    }


    template<typename Item, int Id>
    void writeSymbol(std::ostream & os, Optional<Id, Item>)
    {
        os << Item() << "?";
    }

    template<int Id>
    void writeSymbol(std::ostream & os, Tok<Id>)
    {
        os << Id;
    }


    template<typename T>
    void writeSymbol(std::ostream & os, T)
    {
        // A very light and 
        auto name = typeid(T).name();
        while(std::isdigit(*name)) ++name; 
        os << name;
    }

    template<int Token, typename...Rules>
    void writeSymbol(std::ostream & os, token<Token, Rules...>)
    {
        os << Token; // "token<" << Token << ">";
    }

    template<typename...Rules>
    void writeSymbol(std::ostream & os, symbol<Rules...>)
    {
        os << "S";
    }

    template<typename...Rules>
    std::ostream & operator<<(std::ostream & os, symbol<Rules...>)
    {
        return os << "S";
    }

    template<int Position, typename S, typename... Ss>
    struct write_rule<Position, S, Ss...>
    {
        static void write(std::ostream & os)
        {
            if(Position == 0) os << " .";
            os << " ";
            writeSymbol(os, S());
            write_rule<Position-1, Ss...>::write(os);
        }
    };

    template<int Position>
    struct write_rule<Position>
    {
        static_assert(Position<=0, "Invalid position in rule-position");
        static void write(std::ostream & os)
        {
            if(Position == 0) os << " .";
        }
    };

    template<int Lookahead>
    inline void outputLookahead(std::ostream &os)
    {
        if(Lookahead == EndOfStream)
            os << "$";
        else os << Lookahead;
    }

    template<typename S, int Id, typename...Symbols, int Position, int Lookahead>
    std::ostream & operator<<(std::ostream & os, const rule_position<S, rule<Id, Symbols...>, Position, Lookahead> &)
    {
        writeSymbol(os, S());
        os << "<" << Id << "> ->"; // "rule<" << Id << "> ->";
        write_rule<Position, Symbols...>::write(os);
        os << ", ";
        outputLookahead<Lookahead>(os);
        
        return os;
    }

    template<typename... Symbols>
    struct write_rule2;

    template<typename Symbol, typename... Symbols>
    struct write_rule2<Symbol, Symbols...>
    {
        static void write(std::ostream & os)
        {
            os << " " << Symbol();
            write_rule2<Symbols...>::write(os);
        }
    };

    template<>
    struct write_rule2<>
    {
        static void write(std::ostream & os) {}
    };


    template<int Id, typename...Symbols>
    std::ostream & operator<<(std::ostream & os, rule<Id, Symbols...>)
    {
        os << "rule<" << Id << "> ->";
        write_rule2<Symbols...>::write(os);
        return os;
    }


    template<typename S, int Id, typename...Symbols, int Position, int Lookahead>
    std::ostream & operator<<(std::ostream & os, const rule_position<S, token<Id, Symbols...>, Position, Lookahead> &)
    {
        os << "rule<" << Id << "> ->";
        write_rule<Position, token<Id>>::write(os);
        return os << ", " << Lookahead;
    }

    template<int Token, typename Rule>
    std::ostream & operator<<(std::ostream & os, shift<Token, Rule>)
    {
        return os << "s" << Token << "[" << Rule() << "]";
    }

    template<int Token, typename S, typename Rule>
    std::ostream & operator<<(std::ostream & os, reduce<Token, S, Rule>)
    {
        os <<  Rule() << " . , ";
        outputLookahead<Token>(os);
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, syntax_error)
    {
        return os << "<error>";
    }

    inline void output_node(std::ostream & os, node n, int indent)
    {
        for(int i=0; i<indent; ++i) os << "  ";
        os << "Node type=" << n.id();
        
        if(n.isEmptyNode())
        {
            os << ", empty\n";
        }
        else if(n.isToken())
        {
            assert(n.begin()==n.end());

            auto loc = n.getLocation();
            os << ", location=" << loc.row << ":" << loc.col;
            assert(n.tokenLength() == n.str().length());
            os << ", text=\"" << n.c_str() << "\"" << std::endl;
        }
        else
        {
            // Display children
            os << std::endl;
            for(auto i : n)
            {
                if(i.id() != Hidden)
                    output_node(os, i, indent+1);
            }
        }
    }

    inline std::ostream & operator<<(std::ostream & os, node root)
    {
        output_node(os, root, 0);
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const tree & tree)
    {
        if(tree.empty())
            os << "<empty tree>";
        else
            os << tree.root();
        return os;
    }

}
