#pragma once

#include "rules.hpp"
#include "lex.hpp"

#include <iostream>

namespace feiparser
{
    template<int Ch>
    std::ostream & operator<<(std::ostream & os, ch<Ch>)
    {
        return os << "ch<'" << char(Ch) << "'>";
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

    template<typename It>
    std::ostream & operator<<(std::ostream & os, const token_stream<It> & ts)
    {
        os << "[";
        for(auto i : ts)
            os << i;
        os << "]=" << ts.token();
        return os;
    }
}
