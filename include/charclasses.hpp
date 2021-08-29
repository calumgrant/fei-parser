#pragma once

#include "rules.hpp"

namespace feiparser
{
    using utf8_continuation = chrange<128, 191>;
    using utf8_2 = seq<chrange<192, 223>, utf8_continuation>;
    using utf8_3 = seq<chrange<224, 239>, utf8_continuation, utf8_continuation>;
    using utf8_4 = seq<chrange<240, 247>, utf8_continuation, utf8_continuation, utf8_continuation>;
    using utf8 = alt<utf8_2, utf8_3, utf8_4>;

    using digit = chrange<'0','9'>;
    using lowercase = chrange<'a','z'>;
    using uppercase = chrange<'A','Z'>;
    using alpha = alt<lowercase, uppercase>;
    using alnum = alt<alpha, digit>;

    using ws = chalt<' ', '\r', '\t', '\n'>;
}
