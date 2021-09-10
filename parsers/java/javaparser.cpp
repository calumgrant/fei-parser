#define CELLAR_TRACE_PARSER 1
#include <cellar/java.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;


// Helpers - move to a header file when done

template<typename T, int Id=Hidden>
class List : public symbol<T, rule<Id, List<T>, T>> {};

template<typename T, typename Separator, int Id=Hidden>
class Sequence : public symbol<T, rule<Id, Sequence<T, Separator, Id>, Separator, T>> {};

template<int Id, typename T>
class Optional : public symbol<rule<Id>, rule<Id, T>> {};

template<int Id>
class Tok : public rule<Removed, token<Id>> {};

using TypeName = Sequence<token<java::Identifier>, Tok<java::Dot>>;

using SingleTypeImportDeclaration = rule<java::SingleTypeImportDeclaration,
    Tok<java::Import>, TypeName, Tok<java::Semicolon>>;

// ImportDeclaration
// https://docs.oracle.com/javase/specs/jls/se8/html/jls-7.html#jls-ImportDeclaration

using ImportDeclaration = symbol<
    SingleTypeImportDeclaration//,
    //TypeImportOnDemandDeclaration,
    //SingleStaticImportDeclaration,
    //StaticImportOnDemandDeclaration
    >;

using Annotation = token<java::At>;

using PackageModifier = symbol<Annotation>;

// https://docs.oracle.com/javase/specs/jls/se8/html/jls-7.html#jls-PackageDeclaration
using PackageDeclaration = rule<java::Package, 
    Optional<java::PackageModifierList, List<PackageModifier>>,
    token<java::Package>,
    Sequence<token<java::Identifier>, Tok<java::Dot>>,
    Tok<java::Semicolon>
    >;    

using TypeDeclaration = rule<java::TypeDeclaration, token<java::Void>>;

using CompilationUnit = symbol<
    rule<java::CompilationUnit, 
        Optional<java::PackageDeclaration, PackageDeclaration>, 
        Optional<java::ImportDeclarationList, List<ImportDeclaration>>,
        Optional<java::TypeDeclarationList, List<TypeDeclaration>>
        >>;

char_parser java::parser() { return make_parser<::CompilationUnit>(lexer()); }