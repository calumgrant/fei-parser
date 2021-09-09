#include <cellar/xml.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;

class Misc;

class MiscS : public symbol< 
    rule<xml::Misc>,
    rule<xml::Misc, MiscS, Misc>
    > {};


// https://www.w3.org/TR/xml/#NT-document

class prolog : public symbol<> {};

class element : public symbol<> {};

class document : public symbol<
    rule<xml::document, prolog, element, MiscS>
    > {};