
# Plan for the next week

- Containers
  - `range`
  - `unordered_range`
  - `list`
  - `tree`
  - `tuple` - slow and bad
- Operations
  - `make_tree<>::type`
  - `make_list<>::type`
  - `make_range`
  - `insert`
  - `contains`
  - `size`
  - `merge`
  - `balance`

- Subproject: Data structures
  - IntList
  - IntTree
  - TypeList
  - TypeTree
  - Sorted/unsorted
  - Union
  - Visitor (including static methods!)
  - Intersection
  - push_back
  - push_front
  - sorted
- Algorithms: Work on a range of data structures
  - forall
  - visit
  - insert - with comparator
  - union - with comparator
  - rebalance


- Look at shift-reduce conflicts in Java.
  - Report them better

Baseline memory usage
11s compilation, 2.69GB of memory
Down to 4.96 seconds - with modifier = public/private/protected.
  - 10.9 seconds with all modifiers

Down to 3.91 seconds by caching closures. Why not faster!!
  - 12.78 seconds with all modifiers. Slower.

Experiments to try:
1. Look at the diagnostic counts for javabench
  - Fix up potentially_empty_symbol
  - first tag is horrible
2. Remove final vestiges of typesets
2. Remove `hash` and always use `Class::hash`
3. Compute the closure without building it up.
4. Visit whole grammar and cache it as a list of all reachable symbols and tokens
5. Think about int_sets, where each item is encoded as an int: rule number, position, lookahead.
  - Does not seem any faster to be honest.
`item::action` is either: `reduce<Token, Symbol, Rule>`, or `shift<Token>`

- Think about the closure of a *rule* not a state?


- goto_list is still a sequence: How will this work in the `reduce` function??
- gather conflicts as a separate phase for better reporting.

Latest output:
There are 131 states in the state machine 
add_to_closure_tag = 395
build_closure_tag = 0
closure_tag = 142
token_tag = 19
rule_tag = 106
rule_position_tag = 1152
no_tag = 2517
typeset_tag = 84
typeset_insert_tag = 0
typeset_union_tag = 0
typeset_contains_tag = 0
typeset_size_tag = 0
typeset_sort_tag = 0
typeset_sorted_insert_tag = 16
typeset_sorted_union_tag = 35
follow_tag = 183
first_tag = 40
potentially_empty_symbol_tag = 0
getnext_tag = 102
expand_symbol_tag = 599
build_next_token_list_tag = 154
get_next_token_tag = 145
build_goto_list_tag = 206
gather_shift_states_tag = 166
resolve_conflicts_tag = 142
next_action_state_tag = 71
shift_action_tag = 66
goto_tag = 142
tree_tag = 36024

add_to_closure_tag = 395
build_closure_tag = 0
closure_tag = 142
token_tag = 19
rule_tag = 106
rule_position_tag = 1152
no_tag = 2517
typeset_tag = 84
typeset_insert_tag = 0
typeset_union_tag = 0
typeset_contains_tag = 0
typeset_size_tag = 0
typeset_sort_tag = 0
typeset_sorted_insert_tag = 16
typeset_sorted_union_tag = 35
follow_tag = 183
first_tag = 40
potentially_empty_symbol_tag = 0
getnext_tag = 102
expand_symbol_tag = 599
build_next_token_list_tag = 154
get_next_token_tag = 145
build_goto_list_tag = 206
gather_shift_states_tag = 166
resolve_conflicts_tag = 142
next_action_state_tag = 71
shift_action_tag = 66
goto_tag = 142
tree_tag = 36024


Latest profile info:
```
add_to_closure_tag = 261
build_closure_tag = 0
closure_tag = 141
token_tag = 19
rule_tag = 113
rule_position_tag = 1126
no_tag = 2374
typeset_tag = 350
typeset_insert_tag = 540
typeset_union_tag = 0
typeset_contains_tag = 2354
typeset_size_tag = 0
typeset_sort_tag = 0
typeset_sorted_insert_tag = 16
typeset_sorted_union_tag = 35
follow_tag = 142
first_tag = 1243
potentially_empty_symbol_tag = 2001
getnext_tag = 87
expand_symbol_tag = 332
build_next_token_list_tag = 153
get_next_token_tag = 147
build_goto_list_tag = 205
gather_shift_states_tag = 162
resolve_conflicts_tag = 137
next_action_state_tag = 68
shift_action_tag = 64
goto_tag = 142
tree_tag = 36821
```

Think about `maybe_empty` in terms of iteration.



- Validator for
  - grammar
  - parser
  - lexer



- Remove `Closure` from `expand symbol` so that each symbol has its own expansion. Then we'll use `tree_union` to complete the closure.


- Fix build errors on Linux
- Integrate new type_tree library

- Template profiler

- Suspect that insertion is best handled by using typeset_contains<>

- YAML parser

- Think about how to reduce the sizes of things.
  - When constructing a closure, think about reusing/computing the data as much as possible
  - avoid building lists through types.

- General strategy
  - static template_counter<tag>

- Think about how to represent the LALR kernel

- `lalr_kernel<>`
  - Items without lookahead
  - How to gather this into the lookaheads?

- `item` class
  - `int shift_token`
  - `int reduce_token`
  - `bool shifts`
  - `bool reduces`
  - `next` - The next item after shifting one token. A typeset with 0 or 1 elements
  - `rule`
  - `position`
  - `lookahead`
  - `symbol` - The symbol being reduced.
  - `next_symbol` - a token or symbol to the right of the `.`. If at the end 
  - `next_symbols` - a typeset of 

- `impl::symbol<T>`

- `impl::nonterminal<T>` class
  - `potentially_empty`
  - `name = T`
  - `is_userdefined`
  - `name` ???


  - Shifts symbol
    - The next item
  - Shifts token
    - The next item
  - Reduces with lookahead

- `shift_reduce_conflict`
- `reduce_reduce_conflict`

- `state` class
  - `conflicts` - Set of conflicts or `typeset<>`
  - `tokens` - Sorted set of tokens that it accepts
  - `kernal` - Sorted `typeset<>` of items in the state
  - `closure` - 
  - `potential_actions<token>` - a list of viable actions
  - `resolved_action<Token>` - a single action: one of
      `shift<NewState>::state` - the new state
      `reduce<Rule, Symbol>::rule, ::symbol`
      `syntax_error`
      `accept`


- Debug why we can't display a typeset for the Java parser. Is it too big??
- Figure out how to merge the kernel sets in lookaheads in LALR.

- Wrap a state up `state`, for example

```
struct state<members>
{

};
```



- LALR efficiency
  - Reduce the state size
  - Count the number of states in a grammar
  - `grammar_analysis.hpp`
    - Count the number of states.
    - Gather the number of states.
  - Output states nicely (Workitems)
    - Abbreviate names (e.g. only pick out capital letters)
  - error_on_shift_reduce_conflicts<Grammar>().
  - diagnostics.hpp
    - list all states (and count them!)
    - list all conflicts (and, show_which_reductions!)


```
template<typename T>
void outputRuleName(std::ostream & os, T)
{
  std::cout << typeid(T).name();
}
```

- Java parser
  - Dealing with the shift-reduce conflicts in the grammar
    - Problem is the `Annotation` on javaparser.cpp:73. It seems to be ambiguous about how to

- How to debug conflicts:
  - Show the conflicts in "trace mode"

- Sorted typelist - based on constexpr typeinfo::before or just a comparand

- Optimizing performance
  - Don't store location info?
  - Don't store token contents
  - Optimizing the function call: which tokens to expect
  - Optimizing with UTF8
  - Optimizing switches, for example switch on 4 bits?


- Token types
  - Stored, fully
  - Hidden completely
  - Stored without location/text info


- Create a `state` class with members
  - `kernel`
  - `closure`

- Check whether we are an LALR algorithm and how to reduce the states.

- API tests
  - Test parsing istreams. How does the token value work???

- XML parser
  - Nodes should be easily navigable without visitors
  - Ability to tweak the tree
    - Hide tokens that aren't interesting?
    - Mark some nodes as hidden - perhaps by ID = Hidden
    - Optimizations, for example reducing hidden nodes with size 1

- Think about API for hidden nodes.
  - Can we avoid pushing hidden tokens on the stack in the first place??

- But iterating a node when the last (first) node is empty, e.g. "package".

- Avoid storing tokens by id.

- output a tree without including lots of unnecessary header files

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
  - Parse the typename appropriately.

- How do we turn "nodes" into "objects"

- Invalid parse -> empty tree / null root

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