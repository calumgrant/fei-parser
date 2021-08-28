#include "java.hpp"

#include "feiparser.hpp"

using namespace feiparser;

using java = feiparser::ch<'a'>;

char_lexer javalexer = make_lexer<java>();