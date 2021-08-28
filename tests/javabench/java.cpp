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

using hexdigit = chalt<'0','1','2','3','4','5','6','7','8','9','0','a','b','c','d','e','f','A','B','C','D','E','F'>;
using unicodemarker = ch<'u'>;
using unicodeescape = seq<ch<'\\'>, unicodemarker, hexdigit, hexdigit, hexdigit, hexdigit>;

using ccomment = seq<
    ch<'/'>, ch<'*'>, 
    star<alt<notch<'*'>, seq<ch<'*'>, notch<'/'>>>>, 
    ch<'*'>, ch<'/'>>;
using cppcomment = seq<ch<'/'>, ch<'/'>, star<notch<'\n'>> >;
using comment = alt<ccomment, cppcomment>;

using IntegerTypeSuffix = chalt<'l','L'>;
using NonZeroDigit = chrange<'1','9'>;
using Digit = alt<ch<'0'>, NonZeroDigit>;
using DigitOrUnderscore = alt<Digit, ch<'_'>>;
using Underscores = plus<ch<'_'>>;
using DigitsAndUnderscores = plus<DigitOrUnderscore>;
using Digits = alt<Digit, seq<Digit, optional<DigitsAndUnderscores>, Digit>>;
using DecimalNumeral = alt<ch<'0'>, seq<NonZeroDigit, optional<Digits>>, seq<NonZeroDigit, Underscores, Digits>>;

using DecimalIntegerLiteralT = seq<DecimalNumeral, optional<IntegerTypeSuffix>>;

using java = alt<
    token<100, digits>,
    token<Identifier, seq<alpha, star<alnum>>>,
    whitespace<comment>,
    whitespace<plus<chalt<' ','\n','\r','\t','\f'>>>,
    token<Abstract, string<'a', 'b','s','t','r','a','c','t'>>,
    token<Assert, string<'a','s','s','e','r','t'>>,
    token<Boolean, string<'b','o','o','l','e','a','n'>>,
    token<Break, string<'b','r','e','a','k'>>,
    token<Byte, string<'b','y','t','e'>>,
    token<Case, string<'c','a','s','e'>>,
    token<Catch, string<'c','a','t','c','h'>>,
    token<Char, string<'c','h','a','r'>>,

    // TODO: Other keywords
    token<OpenBrace, ch<'{'>>,
    token<CloseBrace, ch<'}'>>,
    token<OpenParen, ch<'('>>,
    token<CloseParen, ch<')'>>,
    token<Comma, ch<','>>,
    token<Dot, ch<'.'>>,
    token<Lt, ch<'<'>>,
    token<Gt, ch<'>'>>,
    token<DecimalIntegerLiteral, DecimalIntegerLiteralT>
    >;

auto javalexer = make_lexer<java>();
