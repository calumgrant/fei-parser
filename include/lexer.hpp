#include "rules.hpp"
#include "simplify.hpp"
#include "lex.hpp"


namespace feiparser
{

template<typename It>
struct lexer_function
{

};

template<typename Rule, typename It>
lexer_function<It> make_lexer();

template<typename It>
class lexer
{
public:
    typedef int(*fn)(It & fist, It last);
    fn f;

    bool regex_match(It first, It last);
    bool regex_find(It &first, It last);
    int lex(It & first, It last);

    struct token
    {

    };
private:
};

template<typename It, typename Rule>
class lex_stream
{
    Rule rule;
    It current, end;
};

}
