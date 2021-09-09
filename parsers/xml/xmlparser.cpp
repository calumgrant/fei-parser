#include <cellar/xml.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;

using Misc = token<123, ch<'x'>>;

class MiscS : public symbol< 
    rule<xml::Misc>,
    rule<xml::Misc, MiscS, Misc>
    > {};


using XmlDecl = rule<xml::XmlDecl, token<xml::XmlDeclToken>>;

// https://www.w3.org/TR/xml/#NT-prolog
class prolog : public symbol<
    XmlDecl
    > {};

class element : public symbol<> {};

// https://www.w3.org/TR/xml/#NT-document
class document : public symbol<
    rule<xml::document, prolog, element, MiscS>
    > {};


char_parser xml::parser = make_parser<::document>(xml::lexer);
