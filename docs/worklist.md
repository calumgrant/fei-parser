
# Plan for the next week

- Json
  - Fix nested rules in grammar
  - Remove dummy token rules in symbols
  - Flatten hidden nodes
  - Hide uninteresting tokens - Remove completely from the tree
  - Demo of working with the tree using the API
    - Filtering children maybe
  
- Create a `state` class with members
  - `kernel`
  - `closure`  

- Create an expression parser with precedence
  - Create an evaluator for said expression parser
  - Create a toy language as a demo

- Check whether we are an LALR algorithm and how to reduce the states.

- API tests
  - Test parsing istreams. How does the token value work???
 - Auto-generate the token list from the grammar as an option

- XML parser
  - Nodes should be easily navigable without visitors
  - Ability to tweak the tree
    - Hide tokens that aren't interesting?
    - Mark some nodes as hidden - perhaps by ID = Hidden
    - Optimizations, for example reducing hidden nodes with size 1

When performing a reduction (`writable_tree::reduce`), it walks the children as before. Each time it finds a hidden node (by looking at its id), it resizes it to zero children, sets its children to 0, and adds its count to the length of the parent, minus one. The `node::next()` function skips hidden nodes, so hidden nodes do not contribute to its length.

Nodes can be set (via their id) to `Hidden` which removes the node but exposes its children in the parent, or `Remove` which removes the node entirely (including its children).

Maybe, negative nodes are "hidden" or something? Or any node above 1000 is hidden? Problem is that we need to have distinct token nodes.

- output a tree without including lots of unnecessary stuff

- class `writable_tree`

- Improve node class
  - Filter on subnodes
  - Iterate in forward sequence
  - Visitor (including recursive)

- Java parser

- Tidy up the code a bit

- Check safety of node, for example `c_str()`.
  - Tests for that

- Tidy up API of tree and node.

```
is_user_defined_symbol
static const bool valuetype<equals<Sym, typename Sym::rules>>=

gather_symbols<Grammar>
```

- Ensure that `node` is 100% safe to use.
  - Think about a `raw_node` class

- Better sytax error reporting
  - Don't expose "success" and have a writable_tree
  - Report list of tokens it would accept.
  - Better strings in the syntax tree.

- friendly type-name
  - Parse the string appropriately.

# Other stuff to do

- Can you combine ints and types in a rule - ints mean "token" ???

- Tests for well-formedness

- Proper handling of unicode

- Push parser
  - Needs a push_lexer as well

```
auto pl = lexer.start();

pl.push(start, end, fn);
pl.end(fn);

auto pp = parser.start();
// Pushes one token
pp.push(tokeniser);

if(pp.tree.success)
{
  // Blah blah blah
}


pp.push(...)



```

- Restartable parser
  - Push tokens instead 


- Fix how to specify precedence and associativity

- Better utf16 handling
  - What happens if characters overflow??

- Code reorg
  - Create a library with all the parsers
  - Rename to cellar again.
  - Move header files
  - Put private stuff into namespaces

- Parse file

- Different token or node types specify what to store
- Ability to construct the parse tree based on policy or action
- Possibly, the tree could have a virtual method during construction


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

- Lexer actions
- Lexer states

- Operator precedence and associativity