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