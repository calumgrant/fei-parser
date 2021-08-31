
- Remember to reserve space in vectors, e.g. for the parse tree


- Have a seperate actions class - that can be compiled in as a "policy" object
   - This would allow the user to use a supplied grammar without breaking it.
   - Have a datatype on each parse node.

- Avoid infinite recursion

```
template<typename Rule>
struct action
{
    typedef double result_type;
    result_type fn(nodes...)
    {

    }
};


```

- Lexer actions
- Lexer states

- Java lexer
- Java be
