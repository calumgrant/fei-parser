#include "java.hpp"

#include "feiparser.hpp"

using namespace feiparser;

using digit = chrange<'0','9'>;
using digits = plus<digit>;

using java = token<100, digits>;

char_lexer javalexer = make_lexer<java>();
