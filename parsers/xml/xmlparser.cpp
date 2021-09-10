#define CELLAR_TRACE_PARSER 0
#include <cellar/output.hpp>

#include <cellar/xml.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;

using S = symbol<rule<Hidden, token<xml::S>>>;
using Sopt = symbol<rule<Hidden>, rule<Hidden, token<xml::S>>>;

// 23 - https://www.w3.org/TR/xml/#NT-XMLDecl
using XmlDecl = rule<xml::XmlDecl, token<xml::XmlDeclOpen>, S, token<xml::XmlDeclClose>>;

using Comment = token<xml::Comment>;
using PI = token<xml::PI>;
// 27
using Misc = symbol<Comment, PI, S>;

class MiscStar : public symbol<
    rule<xml::Misc>, 
    rule<Hidden, MiscStar, Misc>
    > {};

// https://www.w3.org/TR/xml/#NT-prolog
class prolog : public symbol<
    XmlDecl,
    rule<xml::XmlDecl>
    > {};

class element : public symbol<> {};

// https://www.w3.org/TR/xml/#NT-document
class document : public symbol<
    rule<xml::document, prolog>
    > {};

char_parser xml::parser = make_parser<::document>(xml::lexer);
