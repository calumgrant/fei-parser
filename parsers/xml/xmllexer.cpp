/*
    Lexical analysier for XML.

    See https://www.w3.org/TR/xml/
*/

#include <cellar/xml.hpp>
#include <cellar/make_lexer.hpp>

using namespace cellar;

using S = plus<chalt<' ', '\r', '\n', '\t'>>;

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

// using xml = seq<chalt<'x','X'>, chalt<'m','M'>, chalt<'l', 'L'>>;
using XmlDeclOpen = token<xml::XmlDeclOpen, string<'<', '?', 'x', 'm', 'l'>>;
using XmlDeclClose = token<xml::XmlDeclClose, string<'?', '>'>>;
using SToken = token<xml::S, S>;

using Tokens = alt< XmlDeclOpen, XmlDeclClose, SToken >;

char_lexer xml::lexer = make_lexer<Tokens>();
