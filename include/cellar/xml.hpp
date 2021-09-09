#include <cellar/lexer.hpp>
#include <cellar/parser.hpp>

namespace cellar::xml
{
    enum Nodes
    {
        // Elements
        document,

        Element,
        Attribute,
        Name,
        Value,
        Content,
        CharData,
        Reference,
        CDSect,
        PI,
        Misc,
        ElementDecl,
        ContentSpec,

        XmlDecl,
        XmlDeclOpen,
        XmlDeclClose, 

        // Tokens
        OpenAngle,
        CloseEmpty,

        // Note: Assigned elements need to go at the end to avoid duplicates
        S,
        Comment,
        STag,
        XmlDocument
    };

    extern cellar::char_lexer lexer;
    extern cellar::char_parser parser;
}
