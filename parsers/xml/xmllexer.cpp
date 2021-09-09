/*
    Lexical analysier for XML.

    See https://www.w3.org/TR/xml/
*/

#include <cellar/xml.hpp>
#include <cellar/make_lexer.hpp>

using namespace cellar;

using S = token<Whitespace, plus<chalt<' ', '\r', '\n', '\t'>>>;

using NameStartChar = alt<
    chalt<':', '_'>,
    chrange<'A','Z'>,
    chrange<'a','Z'>
    >;

using NameChar = alt<
    NameStartChar,
    chalt<'-', '.'>,
    ch<0xb7>
    >;

using xml = seq<chalt<'x','X'>, chalt<'m','M'>, chalt<'l', 'L'>>;
