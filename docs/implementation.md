# Implementation of the feiparser

feiparser (fast, easily-integrated parser) is a parser library for C++, that is implemented using C++ templates.

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

Feiparser implements an LALR parser in C++ templates. The computation of the LR "tables" is performed by the C++ compiler. See [] for an overview of LR parsing.

The parser has a stack of states, and a stack of nodes. The stack of nodes is stored in a `tree`.

At each state, the parser looks at the next token, and either

- shifts the token, pushing the state on the stack onto the stack, and going to the next state from the table.
- reduces, pushing a "rule" on the stack, then looks up the next state to go to from the stack.
- error, syntax error
- accepts, finishes the parse

States are encoded as a `typeset<>` consisting of a list of rules and positions. Each rule and position ("item") is encoded as a `rule_position<Rule, Position, Lookahead>`, where each rule is encoded as a terminal (`token<>`) or non-terminal symbol.

Non-terminal symbols contain a "rules" member, where each rule is of the form:

- `token<Id>`
- alt<Rules...>
- rule<Id, Rules...>

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



