#include <cellar/xml.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;

using Misc = token<123, ch<'x'>>;

class MiscS : public symbol< 
    rule<xml::Misc>,
    rule<xml::Misc, MiscS, Misc>
    > {};

using S = token<xml::S>; // rule<100, token<xml::S>>;

using XmlDecl = rule<xml::XmlDecl, token<xml::XmlDeclOpen>, S, token<xml::XmlDeclClose>>;

// https://www.w3.org/TR/xml/#NT-prolog
class prolog : public symbol<
    XmlDecl
    > {};

class element : public symbol<> {};

// https://www.w3.org/TR/xml/#NT-document
class document : public symbol<
    rule<xml::document, prolog>
    > {};


char_parser xml::parser = make_parser<::document>(xml::lexer);
