/*
 A lexical analyser for Java written using the fei-parser.
 
 This is in a separate file and can be compiled seperately.
 The generated parser is the `javalexer` object.
 
 The lexer is specified using C++ datatypes.
 */


#include "java.hpp"

#include "feiparser.hpp"

using namespace feiparser;

using digit = chrange<'0','9'>;
using digits = plus<digit>;

using javaOld = token<100, digits>;

using digit = chrange<'0','9'>;
using alpha  = alt<chrange<'a','z'>, chrange<'A','Z'>>;
using digits = plus<digit>;
using alnum = alt<alpha, digit>;




using ccomment = seq<
    ch<'/'>, ch<'*'>, 
    star<alt<notch<'*'>, seq<ch<'*'>, notch<'/'>>>>, 
    ch<'*'>, ch<'/'>>;
using cppcomment = seq<ch<'/'>, ch<'/'>, star<notch<'\n'>> >;
using comment = alt<ccomment, cppcomment>;



// 3.1 Unicode

// 3.3 Unicode Escapes

using UnicodeMarker = plus<ch<'u'>>;
using HexDigit = alt<chrange<'0','9'>,chrange<'a','f'>,chrange<'A','F'>>;
using UnicodeEscape = seq<ch<'\\'>, UnicodeMarker, HexDigit, HexDigit, HexDigit, HexDigit>;

// 3.6 White Space

using WhiteSpace = chalt<' ','\n','\r','\t','\f'>;
using WhiteSpaceToken = whitespace<plus<WhiteSpace>>;

// 3.7 Comments

using NotStar = notch<'*'>;
using NotStarNotSlash  = notch<'*','/'>;
using InputCharacter = notch<'\n','\r'>;

/*
// Need to rewrite the grammar.
// Morally was this:
// using CommentTailStar = alt<ch<'/', seq<ch<'*'>, CommentTailStar>
// using CommentTail = alt<seq<NotStar, CommentTail>, seq<ch<'*'>, CommentTailStar>;

*/

using CommentBody = star<alt<NotStar, seq<plus<ch<'*'>>, NotStarNotSlash>>>;
using TraditionalComment = seq<ch<'/'>, ch<'*'>, CommentBody>; // , ch<'*'>, ch<'/'>>;


using EndOfLineComment = seq<ch<'/'>, ch<'/'>, star<InputCharacter>>;
using Comment = alt<TraditionalComment, EndOfLineComment>;
using CommentToken = whitespace<Comment>;
// using CommentToken = token<1000, TraditionalComment>;

using IntegerTypeSuffix = chalt<'l','L'>;
using NonZeroDigit = chrange<'1','9'>;
using Digit = alt<ch<'0'>, NonZeroDigit>;
using DigitOrUnderscore = alt<Digit, ch<'_'>>;
using Underscores = plus<ch<'_'>>;
using DigitsAndUnderscores = plus<DigitOrUnderscore>;
using Digits = alt<Digit, seq<Digit, optional<DigitsAndUnderscores>, Digit>>;
using DecimalNumeral = alt<ch<'0'>, seq<NonZeroDigit, optional<Digits>>, seq<NonZeroDigit, Underscores, Digits>>;

using DecimalIntegerLiteralT = seq<DecimalNumeral, optional<IntegerTypeSuffix>>;

using BooleanLiteralT = alt<string<'t','r','u','e'>, string<'f','a','l','s','e'>>;

using NullLiteralT = string <'n','u','l','l'>;

using KeywordToken = alt<
    token<Abstract, string<'a', 'b','s','t','r','a','c','t'>>,
    token<Assert, string<'a','s','s','e','r','t'>>,
    token<Boolean, string<'b','o','o','l','e','a','n'>>,
    token<Break, string<'b','r','e','a','k'>>,
    token<Byte, string<'b','y','t','e'>>,
    token<Case, string<'c','a','s','e'>>,
    token<Catch, string<'c','a','t','c','h'>>,
    token<Char, string<'c','h','a','r'>>
>;

using SymbolToken = alt<
    token<OpenBrace, ch<'{'>>,
    token<CloseBrace, ch<'}'>>,
    token<OpenParen, ch<'('>>,
    token<CloseParen, ch<')'>>,
    token<Comma, ch<','>>,
    token<Dot, ch<'.'>>,
    token<Lt, ch<'<'>>,
    token<Gt, ch<'>'>>
    // TODO
>;

using java = alt<
    token<100, digits>,
    token<Identifier, seq<alpha, star<alnum>>>,
    CommentToken,
    WhiteSpaceToken,
    //whitespace<comment>,
    // whitespace<plus<chalt<' ','\n','\r','\t','\f'>>>,

    // TODO: Other keywords
    KeywordToken,
    WhiteSpaceToken,
    SymbolToken,



    token<DecimalIntegerLiteral, DecimalIntegerLiteralT>,
    token<BooleanLiteral, BooleanLiteralT>,
    token<NullLiteral, NullLiteralT>
    >;

auto javalexer = make_lexer<java>();
