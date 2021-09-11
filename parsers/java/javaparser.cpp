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

    template<int Id>
    class Tok : public rule<Removed, token<Id>> {};
}

using TODO = symbol<>;

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

using Annotation = TODO;

using PackageModifier = symbol<Annotation>;

// https://docs.oracle.com/javase/specs/jls/se8/html/jls-7.html#jls-PackageDeclaration
using PackageDeclaration = rule<java::Package, 
    Optional<java::PackageModifierList, List<PackageModifier>>,
    token<java::Package>,
    Sequence<token<java::Identifier>, Tok<java::Dot>>,
    Tok<java::Semicolon>
    >;    

// 7.6 Top Level Type Declarations




using EnumDeclaration = TODO;
using NormalClassDeclaration = TODO;


using InterfaceDeclaration = TODO;

class ClassDeclaration : public symbol<NormalClassDeclaration, EnumDeclaration> {};

class TypeDeclaration : public symbol<ClassDeclaration, InterfaceDeclaration, Tok<java::Semicolon>> {};




using CompilationUnit = symbol<
    rule<java::CompilationUnit, 
        Optional<java::PackageDeclaration, PackageDeclaration>, 
        Optional<java::ImportDeclarationList, List<ImportDeclaration>>,
        Optional<java::TypeDeclarationList, List<TypeDeclaration>>
        >>;

char_parser java::parser() { return make_parser<::CompilationUnit>(lexer()); }