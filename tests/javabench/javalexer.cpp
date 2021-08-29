/*
 A lexical analyser for Java written using the fei-parser.
 
 This is in a separate file and can be compiled seperately.
 The generated parser is the `javalexer` object.
 
 The lexer is specified using C++ datatypes.
 */


#include "java.hpp"

#include "feiparser.hpp"

using namespace feiparser;
using namespace JavaParser;

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
using CommentBody = star<alt<NotStar, seq<plus<ch<'*'>>, NotStarNotSlash>>>;
using TraditionalComment = seq<ch<'/'>, ch<'*'>, CommentBody, plus<ch<'*'>>, ch<'/'>>;
using EndOfLineComment = seq<ch<'/'>, ch<'/'>, star<InputCharacter>>;
using Comment = alt<TraditionalComment, EndOfLineComment>;
using CommentToken = whitespace<Comment>;

// 3.8 Identifiers

using JavaLetter = alt<utf8, alpha, ch<'$'>, ch<'_'>>;
using JavaLetterOrDigit = alt<JavaLetter, digit>;
using IdentifierChars = seq<JavaLetter, star<JavaLetterOrDigit>>;
using IdentifierToken = token<Identifier, IdentifierChars>;

// 3.9 Keywords

using KeywordToken = alt<
    token<Abstract, string<'a', 'b','s','t','r','a','c','t'>>,
    token<Assert, string<'a','s','s','e','r','t'>>,
    token<Boolean, string<'b','o','o','l','e','a','n'>>,
    token<Break, string<'b','r','e','a','k'>>,
    token<Byte, string<'b','y','t','e'>>,
    token<Case, string<'c','a','s','e'>>,
    token<Catch, string<'c','a','t','c','h'>>,
    token<Char, string<'c','h','a','r'>>,
    token<Class, string<'c','l','a','s','s'>>,
    token<Const, string<'c','o','n','s','t'>>,

    token<Continue, string<'c','o','n','t','i','n','u','e'>>,
    token<Default, string<'d','e','f','a','u','l','t'>>,
    token<Do, string<'d','o'>>,
    token<Double, string<'d','o','u','b','l','e'>>,
    token<Else, string<'e','l','s','e'>>,
    token<Enum, string<'e','n','u','m'>>,
    token<Extends, string<'e','x','t','e','n','d','s'>>,
    token<Final, string<'f','i','n','a','l'>>,
    token<Finally, string<'f','i','n','a','l','l','y'>>,
    token<Float, string<'f','l','o','a','t'>>,

// TODO!!

    token<While, string<'w','h','i','l','e'>>
>;

// 3.10 Literals


using IntegerTypeSuffix = chalt<'l','L'>;
using NonZeroDigit = chrange<'1','9'>;
using Digit = alt<ch<'0'>, NonZeroDigit>;
using DigitOrUnderscore = alt<Digit, ch<'_'>>;
using Underscores = plus<ch<'_'>>;
using DigitsAndUnderscores = plus<DigitOrUnderscore>;
using Digits = alt<Digit, seq<Digit, optional<DigitsAndUnderscores>, Digit>>;
using DecimalNumeral = alt<ch<'0'>, seq<NonZeroDigit, optional<Digits>>, seq<NonZeroDigit, Underscores, Digits>>;

using DecimalIntegerLiteralToken = seq<DecimalNumeral, optional<IntegerTypeSuffix>>;

using HexDigitsAndUnderscores = plus<alt<HexDigit, ch<'_'>>>;
using HexDigits = alt<HexDigit, seq<HexDigit, HexDigitsAndUnderscores, HexDigit>>;
using HexNumeral = seq<ch<'0'>, chalt<'x','X'>, HexDigits>;

using HexIntegerLiteral = seq<HexNumeral, optional<IntegerTypeSuffix>>;
using HexIntegerLiteralToken = token<JavaParser::HexIntegerLiteral, ::HexIntegerLiteral>;

using OctalDigit = chrange<'0','7'>;
using OctalDigitOrUnderscore = alt<OctalDigit, ch<'_'>>;
using OctalDigitsAndUnderscores = plus<OctalDigitOrUnderscore>;
using OctalDigits = alt<OctalDigit, seq<OctalDigit, optional<OctalDigitsAndUnderscores>, OctalDigit>>;
using OctalNumeral = seq<ch<'0'>, optional<Underscores>, OctalDigits>;
using OctalIntegerLiteralToken = token<OctalIntegerLiteral, seq<OctalNumeral, optional<IntegerTypeSuffix>>>;

using BinaryDigit = chalt<'0', '1'>;
using BinaryDigitOrUnderscore = alt<BinaryDigit, ch<'_'>>;
using BinaryDigitsAndUnderscores = plus<BinaryDigitOrUnderscore>;
using BinaryDigits = alt<BinaryDigit, seq<BinaryDigit, optional<BinaryDigitsAndUnderscores>, BinaryDigit>>;
using BinaryNumeral = seq<ch<'0'>, chalt<'b','B'>, BinaryDigits>;
using BinaryIntegerLiteralToken = token<BinaryIntegerLiteral, seq<BinaryNumeral, optional<IntegerTypeSuffix>>>;

using IntegerLiteralToken = alt<
    DecimalIntegerLiteralToken, HexIntegerLiteralToken, OctalIntegerLiteralToken, BinaryIntegerLiteralToken>;


using BooleanLiteralToken = alt<string<'t','r','u','e'>, string<'f','a','l','s','e'>>;

using NullLiteralToken = string <'n','u','l','l'>;

using LiteralToken = alt<BooleanLiteralToken, NullLiteralToken, IntegerLiteralToken
    // TODO
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
    CommentToken,
    WhiteSpaceToken,
    KeywordToken,
    WhiteSpaceToken,
    SymbolToken,
    IdentifierToken,
    LiteralToken
    >;

auto JavaParser::lexer = make_lexer<java>();
