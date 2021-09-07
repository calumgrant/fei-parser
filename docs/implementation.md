# Implementation of Cellar

Cellar is a parser library for C++, that is implemented using C++ templates. The grammar is specified using C++ syntax

The benefit of this approach is that

- The parser is extremely fast. There are no initialization overheads.
- There are no compromises on the parser algorithm, provided you are happy with LALR.
- The parser is much more tightly integrated with the rest of your code
- Building and integration is much simpler with no special build steps or fighting CMAKE macros
- Avoids all of the frictions commonly associated with integrating parser frameworks
- Generates a parse tree - no need for actions.

My experience with popular parser frameworks for C++ was that they were quite hard to set up and integrate. I also saw a paper that claimed that ANTLR was the fastest parser, which unfortunately did not chime with my own observations, and made me realise that parsing speed was a thing. I was sure I could do better.

My initial feeling was that in order to improve speed (and remove initialization time), we should

- Avoid run-time initialization, preferring compile-time
- Avoid a code generator if possible for ease of building
- Try to specify the grammar in C++ itself
- Prefer header-only library
- Generate a parse tree efficiently
- Generally, avoid expensive operations like memory allocations, type checks and virtual function calls.
- Work with a range of standard C++ idioms like iterators.
- Support parse-tree generation as well as rule actions.
- Use efficient runtime algorithms avoiding backtracking

## The lexical analyser

Specifying the grammar in C++ has a number of pros and cons. On the pro side, there is no separate code generation step, and if using C++ templates, there will not be a run-time overhead. On the negative side, C++ is not the most elegant language for anything, including grammars, but it is possible. Newer C++ features like variadic templates and `using` make this more elegant.

At the heart of the lexical analyser is a "state machine", where each state is encoded by a C++ type. The type is a "rule" defining what language (set of possible strings) are accepted from the state.

- The `reject` state accepts no strings.
- The `empty` state accepts just the empty string.
- The `ch<X>` state accepts a single character `X`.
- The `chrange<A,B>` state accepts any character in the range `A` to `B` inclusive.

The `next<Rule, Ch>` template computes the edges of the state machine. The "result" of this computation is the `type` member -- this is the way "programming with types" works and it looks pretty odd if you haven't see this already.

Here is a simple example of the `next<>` class. C++ templates use pattern matching to find the "most appropriate" template, so `next<ch<'a'>, 'a'>::type` is `empty`, whereas `next<ch<'a'>, 'b'>::type` is `reject`.

```
template<int Ch>
struct next<ch<Ch>, Ch>
{
    typedef empty type;
};

template<int C1, int C2>
struct next<ch<C2>, C2>
{
    typedef reject type;
};
```

The `next<>` template generates the entire state machine: a graph of states and transitions. This state machine is "deterministic" because there is only one `type` defined for each `next<>` otherwise there would be a compile-time error.

Here is how the `next<>` template computes the transitions of an `alt` rule: (sketch)

```
template<typename R1, typename R2, int Ch>
struct next<alt<R1,R2>>
{
    typedef alt<typename next<R1, Ch>::type, typename next<R2, Ch>::type> type;
};
```

Of course things get more complicated with loops (`star`) and sequences (`seq`). In particular, when "unrolling" loops we need to make sure that the algorithm terminates, by only unrolling just enough to reach an existing state. We also want to reduce the size of the graph as much as possible (and avoid infinite expansion), so we use a `normalize<>` template that attepts to simplify states as much as possible, for example `alt<reject, T>` rewrites to `T`, `seq<reject, T>` rewrites to `reject`, and `seq<empty, T>` rewrites to `T`.

We can "unroll" `star<T>` to `alt<empty, seq<T, star<T>>>`.

Once the state graph has been constructed (using `next<>` and `normalize<>`), we can implement a `lex<>()` function that reads one character, then calls `lex<Next>()` on the next state `Next`. We can use tail-recursion and function inlining to ensure that the `lex<>()` function is efficient. In fact, without tail-recursion, the `lex` function is in danger of overflowing the C++ stack, which is really very bad.

Disassembly:

Graph viewer.

## The parser

Cellar implements an LALR parser in C++ templates. The computation of the LR "tables" is performed by the C++ compiler. See [] for an overview of LR parsing.

The parser has a stack of states, and a stack of nodes. The stack of nodes is stored in a `tree`.

At each state, the parser looks at the next token, and either

- shifts the token, pushing the state on the stack onto the stack, and going to the next state from the table.
- reduces, pushing a "rule" on the stack, then looks up the next state to go to from the stack.
- error, syntax error
- accepts, finishes the parse

States are encoded as a `typeset<>` consisting of a list of rules and positions. Each rule and position ("item") is encoded as a `rule_position<Rule, Position, Lookahead>`, where each rule is encoded as a terminal (`token<>`) or non-terminal symbol.

Non-terminal symbols contain a "rules" member, where each rule is of the form:

- `token<Id>`
- symbol<Rules...>
- rule<Id, Symbols...>

## Compiling the parser

For each state (in the `parse` function):
1. Create the "closure" of the state ??
2. Recognise all terminal symbols that can be shited (`. x` in a rule).
3. Recognise all nonterminal symbols that can be shifted (`. X` in a= rule).
For all shifts: Implement as a series of `if`s, that tests the id of the token. The body of the `if` performs the shift, the tail-calls `parse` on the next state.
For all reduces: 
4. Recognise rules that can be reduced (`. {x}` in a rule)
5. Identify conflicts and report them.

Creating the closure: Each state needs to be "closed" meaning that every item contining ". X" needs to add all rules in X to the closure. We need to consider potentially  empty rules

## Running the parser

At the surface level, the compiler has produced a `parse` function that takes a block of text (via pointers/iterators to the text) and produces a parse tree. The tree is stored in a `tree` object, that has been optimized for efficiency.

Internally, the parser uses a `token_stream` generated by the lexical analyser. (See Lexical analysis).

To start parsing, push a `reduce<S0>` function and run the `parse<S0>(state)`, where S0 is the initial state. Note that S0 is a "kernel" and has not been "closed" yet.

The `parse` function checks the current token, and if that token is a shift, performs a tail-call to `parse<S1>`, where S1 = `shift<S0, Tok>::type`. On each shift, the [address of] the relevant `reduce<>` function is pushed onto the stack, and the next token is fetched.

If the token action is a "reduce", then `parseState<>` reduces the stack, and calls the `reduce<>` function on the stack, passing in the relevant typeid of the rule that got reduced.

The `reduce` function checks the id of the rule that got reduced, and calls the relevant `parse` function. (Option: `reduce` exits and the outer loop calls the function, to avoid unbounded recursion. Don't know if the compiler performs tail-recursion through pointer-calls.)

If there is no action for a `parse`, then a syntax error is generated. This marks the output as having a syntax error.

Parsing terminates when
- The action is an "accept", and the tree is marked as "success"
- A syntax error is found.


## Aside: Computing with types

The idea of running algorithms in your compiler might seem odd - indeed a Turing-complete type system does not seem like a sensible idea as it's not "decidable". But in practise it's quite handy, and whenever theory and practise differ, theory must give way.

Here is a type that holds an integer:

```
struct size<int S> {};
```

Here is a type that adds two sizes:

```
template<typename S1, typename S2> struct add_sizes;

template<int S1, int S2> struct add_sizes<size<S1>, size<S2>>
{
    using type = size<S2+S2>;
};
```

Note that you can use `constexpr` functions for this as well, but this only works for const values and not for types.

A "function" in a template could be regarded as a type that transforms a set of types.

By convention, we'll store the result of a type-function in the `type` member. If a type computes a value, then we'll store that in the `value` member.

Type-functions can have multiple outputs. They are true functions as it's not possible (or desirable) for the compiler to have side-effects.

```
template<typename Size> struct measure_size;

template<int Size> struct measure_size<size<Size>>
{
    static const bool length = Size;
    static const bool is_empty = length==0;
    static const bool fits_in_the_car = length <= 2;
};
```

Types can be used to represent complex structures: lists, sets, trees, and arbitrary data structures. Here is a binary tree:

```
template<typename Item> struct leaf;
template<typename Left, typename Right> struct node;
```

Here is a type-function that counts the number of nodes in a tree:

```
template<typename Tree> struct count_nodes;

template<typename Item> struct count_nodes<leaf<Item>>
{
    static const int value = 1;
};

template<typename Left, typename Right> struct count_nodes<node<Left, Right>>
{
    static const int value = 1 + count_nodes<Left>::value + count_nodes<Right>::value;
};
```

You can also do the same thing using `constexpr`, for example

```
template<typename I>
constexpr int tree_size(leaf<I>) { return 1; }

template<typename L, typename R>
constexpr int tree_size(node<L,R>) { return 1 + tree_size(L()) + tree_size(R()); }
```

C++ templates have all of the necessary machinery to write type-functions, using pattern-matching to select the correct function based on matching the structure of a type.

Since C++ templates are Turing-complete, there is no guarantee of termination. This may seem terrible, but all that happens is that compilation fails because it runs out of memory or exceeds a complexity limit. If your program is correct, compilation will succeed. The debugging experience for C++ template metaprogramming isn't all that great to be fair - you can't set breakpoints in the compiler for example.

## Useful parser classes

`typeset<...>` contains a set of types.
- `typeset_insert<Item, Set>::type` inserts one item into a typeset, with the resulting set in `type`.
- `typeset_union<S1,S2>::type` performs a set union of two sets.

`closure<Kernel>::type` computes the LALR "closure" of a state.
- `build_closure<State, Closure>::type` constructs a closure
- `add_to_closure<Item, Closure>::type` adds a new item to the closure, expanding it as required. It only expands items that haven't already been added (to avoid infinite recursion).
- 

`first<Symbol>::type` constructs the FIRST set of tokens that a symbol can match. This needs to 

`follow<Item>::type` constructs the follow-set for an item - the set of all tokens that could follow the next symbol in the item. This needs to take into account potentially empty symbols.

`potentially_empty_symbol<Symbol>::value` computes a true/false value depending on whether `Symbol` can be empty.

Welcome to the wacky world of C++ template metaprogramming! What has made this project particularly challenging, is that LALR parser construction isn't terribly well explained anywhere, and that C++ template metaprogramming isn't terribly easy, particularly if you are trying to implement algorithms. 