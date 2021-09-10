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

using Name = seq<NameStartChar, star<NameChar>>;

// using xml = seq<chalt<'x','X'>, chalt<'m','M'>, chalt<'l', 'L'>>;
using XmlDeclOpen = token<xml::XmlDeclOpen, string<'<', '?', 'x', 'm', 'l'>>;
using XmlDeclClose = token<xml::XmlDeclClose, string<'?', '>'>>;
using SToken = token<xml::S, S>;

// 15
using Comment = seq<
    string<'<','!','-','-'>,
    star<alt<notch<'-'>, seq<ch<'-'>,notch<'-'>>>>,
    string<'-','-','>'>
    >;

/*
Copied from Java
using NotStar = notch<'*'>;
using NotStarNotSlash  = notch<'*','/'>;
using InputCharacter = notch<'\n','\r'>;
using CommentBody = star<alt<NotStar, seq<plus<ch<'*'>>, NotStarNotSlash>>>;
using TraditionalComment = seq<ch<'/'>, ch<'*'>, CommentBody, plus<ch<'*'>>, ch<'/'>>;
using EndOfLineComment = seq<ch<'/'>, ch<'/'>, star<InputCharacter>>;
using Comment = alt<TraditionalComment, EndOfLineComment>;
using CommentToken = whitespace<Comment>;
*/

using NotStar = notch<'?'>;
using NotStarNotSlash  = notch<'?','>'>;
using CommentBody = star<alt<NotStar, seq<plus<ch<'?'>>, NotStarNotSlash>>>;
using PI = seq<ch<'<'>, ch<'?'>, CommentBody, plus<ch<'?'>>, ch<'>'>>;
using PIToken = token<xml::PI, PI>;

using CommentToken = token<xml::Comment, Comment>;

using Tokens = alt< XmlDeclOpen, XmlDeclClose, SToken, CommentToken, PIToken >;


char_lexer xml::lexer() { return make_lexer<Tokens>(); }
