#include <cellar/json.hpp>
#include <cellar/make_lexer.hpp>

using namespace cellar;

using False = string<'f', 'a', 'l', 's', 'e'>;
using True = string<'t', 'r', 'u', 'e'>;


using Tokens = alt <
    token<json::True, True>
    >;
char_lexer json::lexer = make_lexer<Tokens>();
