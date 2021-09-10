#include <cellar/java.hpp>
#include <cellar/make_parser.hpp>

using namespace cellar;

template<typename T, int Id=Hidden>
class List : public symbol<T, rule<Id, List<T>, T>> {};

template<int Id, typename T>
class Optional : public symbol<
    rule<Id>, rule<Id, T>> {};

using PackageDeclaration = rule<java::Package, token<java::Package>>;

using ImportDeclaration = rule<java::ImportDeclaration, token<java::Import>>;

using TypeDeclaration = rule<java::TypeDeclaration, token<java::Void>>;

using CompilationUnit = symbol<
    rule<java::CompilationUnit, 
        Optional<java::PackageDeclaration, PackageDeclaration>, 
        Optional<java::ImportDeclarationList, List<ImportDeclaration>>,
        Optional<java::TypeDeclarationList, List<TypeDeclaration>>
        >>;

char_parser java::parser() { return make_parser<::CompilationUnit>(lexer()); }