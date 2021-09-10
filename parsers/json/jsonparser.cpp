/*
    Definition of a JSON parser in Cellar

    See https://datatracker.ietf.org/doc/html/rfc7159
*/

#define CELLAR_TRACE_PARSER 0
#include <cellar/json.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;

class Value;

using False = token<json::False>;
using Null = token<json::Null>;
using True = token<json::True>;
using Number = token<json::Number>;
using String = token<json::String>;

class Values : public symbol<Value, rule<Hidden, Values, token<','>, Value>> {};

using Array = rule<json::Array, token<'['>, Values, token<']'>>;

using Member = rule<json::Member, String, token<':'>, Value>;
class Members : public symbol<Member, rule<Hidden, Members, token<','>, Member>> {};

using Object = rule<json::Object, token<'{'>, Members, token<'}'>>;

class Value : public symbol<False, Null, True, Object, Array, Number, String> {};

char_parser json::parser = make_parser<Value>(lexer);
