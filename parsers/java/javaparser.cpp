#define CELLAR_TRACE_PARSER 0
#include <cellar/java.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;


// Helpers - move to a header file when done

namespace cellar
{
    template<typename T, int Id=Hidden>
    class List : public symbol<T, rule<Id, List<T>, T>> {};

    template<typename T, typename Separator, int Id=Hidden>
    class Sequence : public symbol<T, rule<Id, Sequence<T, Separator, Id>, Separator, T>> {};

    template<int Id, typename T>
    class Optional : public symbol<rule<Id>, rule<Id, T>> {};

    template<int Id, typename T>
    class OptionalList : public Optional<Id, List<T>> {};

    template<int Id>
    class Tok : public rule<Removed, token<Id>> {};
}

using Identifier = token<java::Identifier>;

using TypeName = Sequence<token<java::Identifier>, Tok<java::Dot>>;

using PackageOrTypeName = Sequence<token<java::Identifier>, Tok<java::Dot>>;

using SingleTypeImportDeclaration = rule<java::SingleTypeImportDeclaration,
    Tok<java::Import>, TypeName, Tok<java::Semicolon>>;

using TypeImportOnDemandDeclaration =
    rule<java::TypeImportOnDemandDeclaration, Tok<java::Import>, PackageOrTypeName, Tok<java::Dot>, Tok<java::Star>, Tok<java::Semicolon>>;

using SingleStaticImportDeclaration = 
    rule<java::SingleStaticImportDeclaration, Tok<java::Import>, Tok<java::Static>, TypeName, Tok<java::Dot>, Identifier, Tok<java::Semicolon>>;

using StaticImportOnDemandDeclaration =
    rule<java::StaticImportOnDemandDeclaration, Tok<java::Import>, Tok<java::Static>, TypeName, Tok<java::Dot>, Tok<java::Star>, Tok<java::Semicolon>>;

// ImportDeclaration
// https://docs.oracle.com/javase/specs/jls/se8/html/jls-7.html#jls-ImportDeclaration

using ImportDeclaration = symbol<
    SingleTypeImportDeclaration,
    TypeImportOnDemandDeclaration,
    SingleStaticImportDeclaration,
    StaticImportOnDemandDeclaration
    >;

using Annotation = token<-1234>;

using PackageModifier = symbol<Annotation>;

// https://docs.oracle.com/javase/specs/jls/se8/html/jls-7.html#jls-PackageDeclaration
using PackageDeclaration = rule<java::Package, 
    Optional<java::PackageModifierList, List<PackageModifier>>,
    Tok<java::Package>,
    Sequence<Identifier, Tok<java::Dot>>,
    Tok<java::Semicolon>
    >;    

// 7.6 Top Level Type Declarations


// https://docs.oracle.com/javase/specs/jls/se8/html/jls-8.html#jls-ClassModifier
using ClassModifier = symbol<
    // Annotation, 
    token<java::Public>,
    token<java::Protected>,
    token<java::Private>,
    token<java::Abstract>,
    token<java::Static>,
    token<java::Final>,
    token<java::Strictfp>
    >;

using TypeVariable = symbol<
    OptionalList<java::AnnotationList, Annotation>, Identifier
    >;

class ClassType;

using InterfaceType = ClassType;
using ClassOrInterfaceType = symbol<ClassType, InterfaceType>;

using FloatingPointType = symbol<token<java::Float>, token<java::Double>>;
using IntegralType = symbol<token<java::Byte>, token<java::Short>, token<java::Int>, token<java::Long>, token<java::Char>>;

using NumericType = symbol<IntegralType, FloatingPointType>;

class PrimitiveType : public symbol<
    rule<java::PrimitiveType, OptionalList<java::AnnotationList, Annotation>, NumericType>,
    rule<java::PrimitiveType, OptionalList<java::AnnotationList, Annotation>, token<java::Boolean>>
    > {};

using Dim = rule<Hidden, OptionalList<java::AnnotationList, Annotation>, Tok<java::OpenSquare>, Tok<java::CloseSquare>>;

using Dims = rule<
    java::Dims,
    List<Dim>
    >;
    

using ArrayType = symbol<
    rule<java::ArrayType, PrimitiveType, Dims>,
    rule<java::ArrayType, ClassOrInterfaceType, Dims>,
    rule<java::ArrayType, TypeVariable, Dims>
    >;

using ReferenceType = symbol<
    ClassOrInterfaceType,
    TypeVariable,
    ArrayType
    >;

using WildcardBounds = symbol<
    rule<java::ExtendsWildcard, Tok<java::Extends>, ReferenceType>,
    rule<java::SuperWildcard, Tok<java::Super>, ReferenceType>
    >;

using Wildcard = rule<java::Wildcard, 
    OptionalList<java::AnnotationList, Annotation>,
    Tok<java::Question>,
    Optional<Hidden, WildcardBounds>
    >;

using TypeArgument = symbol<
    ReferenceType,
    Wildcard
    >;

using TypeArgumentList = Sequence<TypeArgument, Tok<java::Comma>>;

using TypeArguments = rule<java::TypeArgumentList,
    Tok<java::Lt>, TypeArgumentList, Tok<java::Gt>
    >;

class ClassType : public symbol <
    rule<java::ClassType, 
        OptionalList<java::AnnotationList, Annotation>,
        Identifier,
        Optional<java::TypeArgumentList, TypeArguments>>,
    rule<java::ClassType,
        ClassOrInterfaceType, Tok<java::Dot>, OptionalList<java::AnnotationList, Annotation>, 
        Identifier,
        Optional<java::TypeArgumentList, TypeArguments>>
    > {};

using InterfaceType = ClassType;

using AdditionalBound = rule<java::AdditionalBound, Tok<java::Amp>, InterfaceType>;

using TypeBound = symbol<
    rule<java::TypeBoundExtendsVar, Tok<java::Extends>, TypeVariable>,
    rule<java::TypeBoundExtendsClassOrInterface, Tok<java::Extends>, Optional<java::AdditionalBound, AdditionalBound>>
    >;

using TypeParameter = rule<
    java::TypeParameter,
    OptionalList<java::TypeParameterModifierList, Annotation>,
    Identifier,
    List<TypeBound>
    >;

using TypeParameters = rule<java::TypeParameterList,
    Tok<java::Lt>,
    Sequence<TypeParameter, Tok<java::Comma>>, 
    Tok<java::Gt>
    >;

//template<> class cellar::ignore_shift_reduce_conflict<49, 130> : public true_value {};
//template<> class cellar::ignore_shift_reduce_conflict<-5, 130> : public true_value {};

using Superclass = rule<java::Superclass, Tok<java::Extends>, ClassType>;

using Superinterfaces = rule<java::Superinterfaces, Tok<java::Implements>, Sequence<InterfaceType, Tok<java::Comma>>>;

class ClassBody : public symbol<
    rule<java::ClassBody, Tok<java::OpenBrace>, /* TODO */ Tok<java::CloseBrace>> 
    >
    {};

// https://docs.oracle.com/javase/specs/jls/se8/html/jls-8.html#jls-NormalClassDeclaration
using NormalClassDeclaration = rule<
    java::NormalClassDeclaration,
    OptionalList<java::ClassModifierList, ClassModifier>,
    Tok<java::Class>,
    Identifier,
    /* Optional<java::TypeParameterList, TypeParameters>, */
    /* Optional<java::Superclass, Superclass>, */
    /* Optional<java::Superinterfaces, Superinterfaces>, */
    ClassBody
    >;

class ClassDeclaration : public symbol<NormalClassDeclaration /*, EnumDeclaration*/> {};

class TypeDeclaration : public symbol<ClassDeclaration, /* InterfaceDeclaration, */ Tok<java::Semicolon>> {};

using CompilationUnit = symbol<
    rule<java::CompilationUnit, 
        Optional<java::PackageDeclaration, PackageDeclaration>, 
        Optional<java::ImportDeclarationList, List<ImportDeclaration>>,
        Optional<java::TypeDeclarationList, List<TypeDeclaration>>
        >>;

char_parser java::parser() { return make_parser<::CompilationUnit>(lexer()); }