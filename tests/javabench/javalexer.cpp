/*
    A lexical analyser for Java written using the fei-parser.
    
    This is in a separate file and can be compiled seperately.
    The generated parser is the `JavaParser::lexer` object.
    
    The lexer is specified using C++ datatypes, and compiled using just a C++ compiler.

    See the Java Language Specification Chapter 3 for the specification of the 
    lexical analyser.
    https://docs.oracle.com/javase/specs/jls/se8/html/jls-3.html

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

// Incorrect here as utf8 should be preprocessed
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

    token<For, string<'f','o','r'>>,
    token<If, string<'i','f'>>,
    token<Goto, string<'g','o','t','o'>>,
    token<Implements, string<'i','m','p','l','e','m','e','n','t','s'>>,
    token<Import, string<'i','m','p','o','r','t'>>,
    token<Instanceof, string<'i','n','s','t','a','n','c','e','o','f'>>,
    token<Int, string<'i','n','t'>>,
    token<Interface, string<'i','n','t','e','r','f','a','c','e'>>,
    token<Long, string<'l','o','n','g'>>,
    token<Native, string<'n','a','t','i','v','e'>>,

    token<New, string<'n','e','w'>>,
    token<Package, string<'p','a','c','k','a','g','e'>>,
    token<Private, string<'p','r','i','v','a','t','e'>>,
    token<Protected, string<'p','r','o','t','e','c','t','e','d'>>,
    token<Public, string<'p','u','b','l','i','c'>>,
    token<Return, string<'r','e','t','u','r','n'>>,
    token<Short, string<'s','h','o','r','t'>>,
    token<Static, string<'s','t','a','t','i','c'>>,
    token<Strictfp, string<'s','t','r','i','c','t','f','p'>>,
    token<Super, string<'s','u','p','e','r'>>,

    token<Switch, string<'s','w','i','t','c','h'>>,
    token<Synchronized, string<'s','y','n','c','h','r','o','n','i','z','e','d'>>,
    token<This, string<'t','h','i','s'>>,
    token<Throw, string<'t','h','r','o','w'>>,
    token<Transient, string<'t','r','a','n','s','i','e','n','t'>>,
    token<Try, string<'t','r','y'>>,
    token<Void, string<'v','o','i','d'>>,
    token<Volatile, string<'v','o','l','a','t','i','l','e'>>,
    token<While, string<'w','h','i','l','e'>>
    >;

// 3.10 Literals

// 3.10.1 Integer literals

using IntegerTypeSuffix = chalt<'l','L'>;
using NonZeroDigit = chrange<'1','9'>;
using Digit = alt<ch<'0'>, NonZeroDigit>;
using DigitOrUnderscore = alt<Digit, ch<'_'>>;
using Underscores = plus<ch<'_'>>;
using DigitsAndUnderscores = plus<DigitOrUnderscore>;
using Digits = alt<Digit, seq<Digit, optional<DigitsAndUnderscores>, Digit>>;
using DecimalNumeral = alt<ch<'0'>, seq<NonZeroDigit, optional<Digits>>, seq<NonZeroDigit, Underscores, Digits>>;

using DecimalIntegerLiteralToken = token<DecimalIntegerLiteral, seq<DecimalNumeral, optional<IntegerTypeSuffix>>>;

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
    DecimalIntegerLiteralToken,
    HexIntegerLiteralToken,
    OctalIntegerLiteralToken,
    BinaryIntegerLiteralToken
    >;

// 3.10.2 Floating point literals

using Sign = chalt<'+', '-'>;
using FloatTypeSuffix = chalt<'f', 'F', 'd', 'D'>;
using SignedInteger = seq<optional<Sign>, Digits>;
using ExponentIndicator = chalt<'e','E'>;
using ExponentPart = seq<ExponentIndicator, SignedInteger>;

using DecimalFloatingPointLiteral = alt<
    seq<Digits, ch<'.'>, optional<Digits>, optional<ExponentPart>, optional<FloatTypeSuffix>>,
    seq<ch<'.'>, Digits, optional<ExponentPart>, optional<FloatTypeSuffix>>,
    seq<Digits, ExponentPart, optional<FloatTypeSuffix>>,
    seq<Digits, optional<ExponentPart>, FloatTypeSuffix>
    >;

using HexSignificand = alt<
    seq<HexNumeral, optional<ch<'.'>>>,
    seq<ch<'0'>, chalt<'x', 'X'>, optional<HexDigits>, ch<'.'>, HexDigits>
    >;

using BinaryExponentIndicator = chalt<'p', 'P'>;
using BinaryExponent = seq<BinaryExponentIndicator, SignedInteger>;

using HexadecimalFloatingPointLiteral = seq<HexSignificand, BinaryExponent, optional<FloatTypeSuffix>>;

using FloatingPointLiteralToken = alt<
    token<JavaParser::DecimalFloatingPointLiteral, ::DecimalFloatingPointLiteral>,
    token<JavaParser::HexadecimalFloatingPointLiteral, ::HexadecimalFloatingPointLiteral>
    >;

// 3.10.3 Boolean Literals

using BooleanLiteralToken = token<BooleanLiteral, alt<string<'t','r','u','e'>, string<'f','a','l','s','e'>>>;

// 3.10.4 Charactar Literals

using SingleCharacter = notch<'\r', '\n', '\'', '\\'>;

using ZeroToThree = chrange<'0', '3'>;

using OctalEscape = alt<
    seq<ch<'\\'>, OctalDigit>,
    seq<ch<'\\'>, OctalDigit, OctalDigit>,
    seq<ch<'\\'>, ZeroToThree, OctalDigit, OctalDigit>
    >;

using EscapeSequence = alt<
    UnicodeEscape, // Standards violation as this should be done by the preprocessor
    string<'\\', 'b'>,
    string<'\\', 't'>,
    string<'\\', 'n'>,
    string<'\\', 'f'>,
    string<'\\', 'r'>,
    string<'\\', '\"'>,
    string<'\\', '\''>,
    string<'\\', '\\'>,
    OctalEscape
    >;

using CharacterLiteral = alt<
    seq<ch<'\''>, SingleCharacter, ch<'\''>>,
    seq<ch<'\''>, EscapeSequence, ch<'\''>>
    >;

using CharacterLiteralToken = token<JavaParser::CharacterLiteral, ::CharacterLiteral>;

// 3.10.5 String Literals

using StringCharacter = alt<
    notch<'\r', '\n', '\"', '\\'>,
    EscapeSequence
    >;

using StringBlockTerminator = string<'\"','\"','\"'>;

using TextBlockCh = 
    seq<seq<optional<ch<'\"'>>, optional<ch<'\"'>>>, notch<'\"'>>;

// Not in the standard, but a proposal needed to parse the JDK
using TextBlock = seq<StringBlockTerminator, star<TextBlockCh>, StringBlockTerminator>;

using StringLiteral = alt<
    seq<ch<'\"'>, star<StringCharacter>, ch<'\"'>>,
    TextBlock>;
using StringLiteralToken = token<JavaParser::StringLiteral, ::StringLiteral>;

// 3.10.7 The null literal

using NullLiteralToken = token<NullLiteral, string <'n','u','l','l'>>;

using LiteralToken = alt<
    BooleanLiteralToken,
    NullLiteralToken,
    IntegerLiteralToken,
    FloatingPointLiteralToken,
    CharacterLiteralToken,
    StringLiteralToken
    >;

// 3.11 Separators

using SeparatorToken = alt<
    token<OpenParen, ch<'('>>,
    token<CloseParen, ch<')'>>,
    token<OpenBrace, ch<'{'>>,
    token<CloseBrace, ch<'}'>>,
    token<OpenSquare, ch<'['>>,
    token<CloseSquare, ch<']'>>,
    token<Semicolon, ch<';'>>,
    token<Comma, ch<','>>,
    token<Dot, ch<'.'>>,
    token<DotDotDot, string<'.', '.', '.'>>,
    token<At, ch<'@'>>,
    token<ColonColon, string<':', ':'>>
>;

// 3.12 Operators

using OperatorToken = alt<
    token<Eq, ch<'='>>,
    token<EqEq, string<'=', '='>>,
    token<Plus, ch<'+'>>,
    token<PlusEq, string<'+', '='>>,
    token<Gt, ch<'>'>>,
    token<GtEq, string<'>', '='>>,
    token<Minus, ch<'-'>>,
    token<MinusEq, string<'-', '='>>,
    token<Lt, ch<'<'>>,
    token<LtEq, string<'<','='>>,
    token<Times, ch<'*'>>,
    token<TimesEq, string<'*', '='>>,
    token<Not, ch<'!'>>,
    token<NotEq, string<'!','='>>,
    token<Div, ch<'/'>>,
    token<DivEq, string<'/','='>>,
    token<Tilde, string<'~'>>,
    token<AmpAmp, string<'&','&'>>,
    token<Amp, ch<'&'>>,
    token<AmpEq, string<'&','='>>,
    token<Question, ch<'?'>>,
    token<OrOr, string<'|','|'>>,
    token<Or, ch<'|'>>,
    token<OrEq, string<'|','='>>,
    token<Colon, ch<':'>>,
    token<PlusPlus, string<'+','+'>>,
    token<Hat, ch<'^'>>,
    token<HatEq, string<'^','='>>,
    token<Arrow, string<'-','>'>>,
    token<MinusMinus, string<'-','-'>>,
    token<Mod, ch<'%'>>,
    token<ModEq, string<'%','='>>,
    token<LeftShift, string<'<','<'>>,
    token<LeftShiftEq, string<'<','<','='>>,
    token<RightShift, string<'>','>'>>,
    token<RightShiftEq, string<'>','>','='>>,
    token<RRightShift, string<'>','>','>'>>,
    token<RRightShiftEq, string<'>','>','>','='>>
>;

#if NDEBUG

using JavaTokens = alt<
    CommentToken,
    WhiteSpaceToken,
    KeywordToken,
    WhiteSpaceToken,
    IdentifierToken,
    LiteralToken,
    SeparatorToken,
    OperatorToken
    >;

#else

// This is just to speed up compilation during development

using JavaTokens = alt<
    // CommentToken,
    //WhiteSpaceToken,
    KeywordToken,
    //WhiteSpaceToken,
    //IdentifierToken,
    //LiteralToken,
    SeparatorToken,
    OperatorToken
    >;

#endif

auto JavaParser::lexer = make_lexer<JavaTokens>();
