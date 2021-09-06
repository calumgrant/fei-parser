

- Parser implementation
  - Check 
  - Code to output a table

- Code reorg
  - Create a library with all the parsers
  - Rename to cellar again.
  - Move header files
  - Put private stuff into namespaces

- Parse file

- Different token or node types specify what to store
- Ability to construct the parse tree based on policy or action
- Possibly, the tree could have a virtual method during construction

- mark and whitelist conflicts

```
struct default_node_traits
{
    static const bool store_string = true;
    static const bool store_length = true;
    static const bool store_location = true;
    static const bool store_end_location = false;
    static const bool hidden_token = true;
    static const bool hidden_node = 

    struct extra_data
    {
        extra_data(node &) {}
    };
};

template<int Id>
struct token_traits : public default_token_traits {};

struct hidden_node : public default_token_traits
{
    static const bool store_string = false;
    static const bool store_length = false;
    static const bool store_location = false;
    static const bool store_end_location = false;
    static const bool hidden = true;
};

template<>
struct token_traits<EndCurly> : public hidden_node {};
```

- json parser
- xml parser

How does the API look???

tree = parseXMLFile("...");

auto doc = xmlparser.parseFile("foo.xml");
auto rootelement = doc.root();

auto node = root.first<Element>();

for(auto i : node.get<Element>()))

for(auto attrib : node.get<Attribute>())
{
    auto k = attrib.first<Key>();
    if(k)
    k.c_str();
}



parser::parseFile




- Optimize code for `rule<X, token<X, ...>>` to avoid the unnecessary reduce

- Tests for Java Text blocks

- Remember to reserve space in vectors, e.g. for the parse tree


- Have a seperate actions class - that can be compiled in as a "policy" object
   - This would allow the user to use a supplied grammar without breaking it.
   - Have a datatype on each parse node.

- Avoid infinite stack depths

- Reporting syntax errors
- Error recovery

```
template<typename Rule>
struct action
{
    typedef double result_type;
    result_type fn(nodes...)
    {

    }
};

- Locations: Do we want to calculate the location from the offset?




```

# Parser functions

```
template<typename It>
class parse_state
{
    tree t;
    tokens
};

template<typename State, int Lookahead>
void parse2(token_iterator<It> & tokens, tree & output)
{
    // How to shift:
    output.shift(Lookahead, tokens.location());
    using N = next<State, Lookahead>::type;
    tokens.lex();
    return parse<N>(tokens, output);)

    // How to reduce:
    output.reduce(NodeId, count);
    // Lookup the "next" state on the stack somehow...





    Option 2: The call stack contains the parse tree (could overflow :-(

}

template<typename State>
void parse(token_iterator<It> & tokens, tree & output)
{
    auto token = tokens.token();
    switch(token)
    {
        case 12:
            return parse2<State, 12>(tokens, output);
        case 13:
            tree.shift(13, ...);
            return parse2<State, 13>(tokens, output);
        case 30:
            return reduce<State, 30>(tokens, output);
        default:
            tree.parse_error(token.begin());
            // parse error
    }
}
```

- Lexer actions
- Lexer states

- Java lexer
- Java be
