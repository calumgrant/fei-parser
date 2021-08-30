
- Tokenizer
- Java
  - char literals
  - string literals

- Have a seperate actions class - that can be compiled in as a "policy" object
   - This would allow the user to use a supplied grammar without breaking it.
   - Have a datatype on each parse node.

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
