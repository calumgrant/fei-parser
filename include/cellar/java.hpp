#include <cellar/lexer.hpp>
#include <cellar/parser.hpp>

namespace cellar::java
{
    enum Tokens
    {
        Abstract,
        Assert,
        Boolean,
        Break,
        Byte,
        Case,
        Catch,
        Char,
        Class,
        Const,
        Continue,
        Default,
        Do,
        Double,
        Else,
        Enum,
        Extends,
        Final,
        Finally,
        Float,
        For,
        If,
        Goto,
        Implements,
        Import,
        Instanceof,
        Int,
        Interface,
        Long,
        Native,
        New,
        Package,
        Private,
        Protected,
        Public,
        Return,
        Short,
        Static,
        Strictfp,
        Super,
        Switch,
        Synchronized,
        This,
        Throw,
        Transient,
        Try,
        Void,
        Volatile,
        While,
        Identifier,  // Note: This must be after the keywords so they take precedence
        DecimalIntegerLiteral,
        HexIntegerLiteral,
        OctalIntegerLiteral,
        BinaryIntegerLiteral,
        DecimalFloatingPointLiteral,
        HexadecimalFloatingPointLiteral,
        CharacterLiteral,
        StringLiteral,
        BooleanLiteral,
        NullLiteral,
        OpenParen,
        CloseParen,
        OpenBrace,
        CloseBrace,
        OpenSquare,
        CloseSquare,
        Semicolon,
        Comma,
        Dot,
        DotDotDot,
        At,
        ColonColon,
        Eq,
        Gt,
        Lt,
        Excl,
        Minus,
        Question,
        Colon,
        Arrow,
        EqEq,
        Plus,
        PlusEq,
        GtEq,
        MinusEq,
        LtEq,
        Times,
        Star=Times,
        TimesEq,
        NotEq,
        Div,
        DivEq,
        Not,
        Tilde,
        AmpAmp,
        Amp,
        AmpEq,
        Or,
        OrOr, OrEq,
        PlusPlus, Hat, HatEq,
        MinusMinus,
        Mod,
        ModEq,
        LeftShift,
        LeftShiftEq,
        RightShift,
        RightShiftEq,
        RRightShift,
        RRightShiftEq,
        LastToken
    };

    enum Nodes
    {
        CompilationUnit = LastToken,
        PackageDeclaration,
        TypeDeclarationList,
        TypeDeclaration,
        ImportDeclarationList,
        ImportDeclaration,
        PackageModifierList,
        SingleTypeImportDeclaration,
        TypeImportOnDemandDeclaration,
        SingleStaticImportDeclaration,
        StaticImportOnDemandDeclaration,
        NormalClassDeclaration,
        ClassModifierList,
        TypeParameter,
        TypeParameterList,
        TypeParameterModifierList,
        TypeBoundExtendsVar,
        TypeBoundExtendsClassOrInterface,
        AdditionalBound,
        AnnotationList,
        ClassType,
        TypeArgumentList,
        ArrayType,
        PrimitiveType,
        Dims,
        Wildcard,
        ExtendsWildcard,
        SuperWildcard,
        Superclass,
        Superinterfaces,
        ClassBody,
        NormalAnnotation,
        ElementValuePairList,
        ElementValuePair,
        TypeBound
    };

    extern cellar::char_lexer lexer();
    extern cellar::char_parser parser();
}
