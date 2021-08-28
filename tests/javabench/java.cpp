#include "java.hpp"

#include "feiparser.hpp"

using namespace feiparser;

using digit = chrange<'0','9'>;
using digits = plus<digit>;

using java = token<100, digits>;

using digit = chrange<'0','9'>;
using alpha  = alt<chrange<'a','z'>, chrange<'A','Z'>>;
using digits = plus<digit>;
using alnum = alt<alpha, digit>;

using java2 = alt<
    token<100, digits>,
    token<101, seq<alpha, star<alnum>>>
    >;

auto javalexer = make_lexer<java2>();
