#pragma once

#include "cellar.hpp"

#include <iostream>

namespace cellar
{
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

    template<typename R1, typename R2>
    std::ostream & operator<<(std::ostream & os, seq<R1,R2>)
    {
        return os << "seq<" << R1() << "," << R2() << '>';
    }

    template<typename R1, typename R2>
    std::ostream & operator<<(std::ostream & os, alt<R1,R2>)
    {
        return os << "alt<" << R1() << "," << R2() << '>';
    }

    std::ostream & operator<<(std::ostream & os, empty)
    {
        return os << "empty";
    }

    std::ostream & operator<<(std::ostream & os, reject)
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
        return os << "chrange<'" << (char)Ch1 << "','" << (char)Ch2 << ">";
    }

    template<int Token, typename Rule>
    std::ostream & operator<<(std::ostream & os, token<Token,Rule>)
    {
        return os << "token<" << Token << "," << Rule() << ">";
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
            os << " " << typename S::rules();
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

    template<typename S, int Id, typename...Symbols, int Position, int Lookahead>
    std::ostream & operator<<(std::ostream & os, const rule_position<S, rule<Id, Symbols...>, Position, Lookahead> &)
    {
        os << "rule<" << Id << "> ->";
        write_rule<Position, Symbols...>::write(os);
        os << ", ";
        if(Lookahead == EndOfStream)
            os << "$";
        else os << Lookahead;
        
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

    template<int Token, typename Rule>
    std::ostream & operator<<(std::ostream & os, reduce<Token, Rule>)
    {
        return os << "r" << Token << "[" << Rule() << "]";
    }

    inline std::ostream & operator<<(std::ostream & os, syntax_error)
    {
        return os << "<error>";
    }
}
