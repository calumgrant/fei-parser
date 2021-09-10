#include <cellar/json.hpp>
#include <cellar/make_lexer.hpp>

using namespace cellar;

using Int = alt<ch<'0'>, seq<chrange<'1', '9'>, star<digit>>>;

using Frac = seq<ch<'.'>, plus<digit>>;

using Exp = seq<chalt<'e', 'E'>, optional<chalt<'+','-'>>, plus<digit>>;

using Number = seq<optional<ch<'-'>>, Int, optional<Frac>, optional<Exp>>;

using unescaped = alt<chrange<0x20, 0x21>, chrange<0x23, 0x5b>, chrange<0x5d, 0x10ffff>>;

using HEX = alt<digit, chrange<'A', 'F'>, chrange<'a', 'f'>>;
using StringChar = alt<unescaped, seq<ch<'\\'>, alt<chalt<'\"', '\\', '/', 'b', 'f', 'r', 'n', 't'>, seq<ch<'u'>, HEX, HEX, HEX, HEX>>>>;
using String = seq<ch<'\"'>, star<StringChar>, ch<'\"'>>;

using Tokens = alt <
    token<'[', ch<'['>>,
    token<']', ch<']'>>,
    token<'{', ch<'{'>>,
    token<'}', ch<'}'>>,
    token<':', ch<':'>>,
    token<',', ch<','>>,
    token<json::True, string<'t', 'r', 'u', 'e'>>,
    token<json::Null, string<'n', 'u', 'l', 'l'>>,
    token<json::False, string<'f', 'a', 'l', 's', 'e'>>,
    token<json::Number, Number>,
    token<json::String, String>
    >;

char_lexer json::lexer() { return make_lexer<Tokens>(); }
