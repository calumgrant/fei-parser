#define CELLAR_TRACE_PARSER 0
#include <cellar/java.hpp>
#include <cellar/make_parser.hpp>
#include <cellar/diagnostics.hpp>

using namespace cellar;


// Helpers - move to a header file when done


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

class Annotation;

using ElementValue = symbol<
    /* ConditionalExpression, */
    /* ElementValueArrayInitializer, */
    Annotation
    >;

using ElementValuePair = rule<java::ElementValuePair, 
    Identifier, Tok<java::Eq>, ElementValue>;

using NormalAnnotation = rule<
    java::NormalAnnotation, 
    Tok<java::At>,
    Tok<java::OpenParen>,
    Optional<java::ElementValuePairList, Sequence<ElementValuePair, Tok<java::Comma>>>,
    Tok<java::CloseParen>
    >;

class Annotation : public symbol<NormalAnnotation /*, MarkerAnnotation, SingleElementAnnotation*/> {};

/*
    In a deviation from the standard grammar, we allow all modifiers on all declarations.
    This makes parsing much easier, but is too permissive.
*/
class Modifier : public symbol<
    // Annotation,
    token<java::Public>,
    token<java::Private>,
    token<java::Protected>,
    token<java::Abstract>,
    token<java::Default>,
    token<java::Static>,
    token<java::Strictfp>,
    token<java::Final>
    > {};

class OptionalModifiers : public OptionalList<java::ModifierList, Modifier> {};

using PackageModifier = symbol<Annotation>;

// https://docs.oracle.com/javase/specs/jls/se8/html/jls-7.html#jls-PackageDeclaration
class PackageDeclaration : public symbol<
    rule<java::Package, 
    // OptionalModifiers,
    // Optional<java::PackageModifierList, List<PackageModifier>>,
        Tok<java::Package>,
        Sequence<Identifier, Tok<java::Dot>>,
        Tok<java::Semicolon>
    >> {};

// 7.6 Top Level Type Declarations


// https://docs.oracle.com/javase/specs/jls/se8/html/jls-8.html#jls-ClassModifier

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
        Identifier
        /*Optional<java::TypeArgumentList, TypeArguments>*/>,
    rule<java::ClassType,
        ClassOrInterfaceType, Tok<java::Dot>, OptionalList<java::AnnotationList, Annotation>, 
        Identifier
        /*Optional<java::TypeArgumentList, TypeArguments>*/>
    > {};

using InterfaceType = ClassType;

using AdditionalBound = rule<java::AdditionalBound, Tok<java::Amp>, InterfaceType>;

using TypeBound = symbol<
    rule<java::TypeBoundExtendsVar, Tok<java::Extends>, TypeVariable>,
    rule<java::TypeBoundExtendsClassOrInterface, Tok<java::Extends>, ClassOrInterfaceType, OptionalList<java::AdditionalBound, AdditionalBound>>
    >;

using TypeParameter = rule<
    java::TypeParameter,
    OptionalList<java::TypeParameterModifierList, Annotation>,
    Identifier,
    Optional<java::TypeBound, TypeBound>
    >;

using TypeParameters = rule<java::TypeParameterList,
    Tok<java::Lt>,
    Sequence<TypeParameter, Tok<java::Comma>>, 
    Tok<java::Gt>
    >;

using Superclass = rule<java::Superclass, Tok<java::Extends>, ClassType>;

using Superinterfaces = rule<java::Superinterfaces, Tok<java::Implements>, Sequence<InterfaceType, Tok<java::Comma>>>;

class ClassBody;
class InterfaceBody;

// https://docs.oracle.com/javase/specs/jls/se8/html/jls-8.html#jls-NormalClassDeclaration
using NormalClassDeclaration = rule<
    java::NormalClassDeclaration,
    // OptionalList<java::AnnotationList, Annotation>,
    OptionalModifiers,
    // OptionalList<java::ClassModifierList, ClassModifier>,
    Tok<java::Class>,
    Identifier,

    // Optional<java::TypeParameterList, TypeParameters>,
    Optional<java::Superclass, Superclass>,
    Optional<java::Superinterfaces, Superinterfaces>,
    ClassBody
    >;

using ExtendsInterfaces = rule<java::ExtendsInterfaces,
    Tok<java::Extends>,
    Sequence<InterfaceType, Tok<java::Comma>>
    >;

using NormalInterfaceDeclaration = rule<
    java::NormalInterfaceDeclaration,
    // OptionalList<java::AnnotationList, Annotation>,
    //OptionalList<java::InterfaceModifierList, ClassModifier>,
    OptionalModifiers,
    Tok<java::Interface>,
    Identifier,
    // Optional<java::TypeParameterList, TypeParameters>,
    Optional<java::Superinterfaces, ExtendsInterfaces>,
    InterfaceBody
    >;


class ClassDeclaration : public symbol<NormalClassDeclaration /*, EnumDeclaration*/> {};

class InterfaceDeclaration : public symbol<NormalInterfaceDeclaration /*, AnnotationTypeDeclaration*/> {};

class TypeDeclaration : public symbol<ClassDeclaration, InterfaceDeclaration, Tok<java::Semicolon>> {};

class CompilationUnit : public symbol<
    rule<java::CompilationUnit, 
        Optional<java::PackageDeclaration, PackageDeclaration>, 
        OptionalList<java::ImportDeclarationList, ImportDeclaration> //,
        // OptionalList<java::TypeDeclarationList, TypeDeclaration>
        >> {};


// Class body
// https://docs.oracle.com/javase/specs/jls/se8/html/jls-8.html#jls-ClassBody        

using ClassMemberDeclaration = symbol<
    /* FieldDeclaration,
    MethodDeclaration, */
    ClassDeclaration,
    InterfaceDeclaration
    >;

using ClassBodyDeclaration = symbol<
    ClassMemberDeclaration /*,
    InstanceInitializer,
    StaticInitializer,
    ConstructorDeclaration */
    >;

using InterfaceMemberDeclaration = symbol<
    /* ConstantDeclaration,
    InterfaceMethodDeclaration, */
    ClassDeclaration,
    InterfaceDeclaration
    >;

using InterfaceBodyDeclaration = symbol<
    InterfaceMemberDeclaration /*,
    InstanceInitializer,
    StaticInitializer,
    ConstructorDeclaration */
    >;


class ClassBody : public symbol<
    rule<java::ClassBody, Tok<java::OpenBrace>, 
        OptionalList<java::ClassBody, ClassBodyDeclaration>, Tok<java::CloseBrace>> 
    >
    {};

class InterfaceBody : public symbol<
    rule<java::InterfaceBody, Tok<java::OpenBrace>, 
        OptionalList<java::ClassBody, InterfaceBodyDeclaration>, Tok<java::CloseBrace>> 
    >
    {};

// Methods

// https://docs.oracle.com/javase/specs/jls/se8/html/jls-9.html#jls-InterfaceMethodDeclaration

/*
class InterfaceMethodDeclaration : public symbol<
    rule<java::InterfaceMethodDeclaration>,
        OptionalModifiers,
        MethodHeader,
        MethodBody>
    > {};
    */

// Conflicts detected so far

template<> struct ignore_shift_reduce_conflict<57, 140> : public true_value {};
template<> struct ignore_shift_reduce_conflict<-6, 140> : public true_value {};
template<> struct ignore_shift_reduce_conflict<42, 121> : public true_value {};
template<> struct ignore_shift_reduce_conflict<41, 121> : public true_value {};


char_parser java::parser()
{
    using Diagnostics = parser_diagnostics<::CompilationUnit>;
    std::cout << Diagnostics::number_of_states << std::endl << Diagnostics::states();

    return make_parser<::CompilationUnit>(lexer());
}