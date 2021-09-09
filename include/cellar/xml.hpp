#include <cellar/parser.hpp>

namespace cellar::xml
{
    enum Nodes
    {
        // Elements
        document = 1,
        Document = document,

        Element,
        Attribute,
        Name,
        Value,
        Content,
        CharData,
        Reference,
        CDSect,
        PI = 16,
        Misc = 27,
        ElementDecl = 45,
        ContentSpec,


        // Tokens
        OpenAngle,
        CloseEmpty,



        // Note: Assigned elements need to go at the end to avoid duplicates
        S = Hidden,
        Comment = Whitespace,
        STag = Hidden,
        XmlDocument = Hidden
    };
}
