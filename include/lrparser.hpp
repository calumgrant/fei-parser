namespace feiparser
{

    // Should really be hidden/moved from here
    template<typename It>
    struct parse_state
    {
        typedef void(*NextFn)(int id, parse_state&);
        std::vector<NextFn> stack;
        tree parse_tree;
        token_stream<It> tokens;
    };

    template<typename Rule, typename It>
    void parsefn(parse_state<It> &)
    {

    }

    template<typename Rule, int Position, int Lookahead>
    struct rule_position {};

    template<typename State>
    struct shift
    {

    };

    template<typename State, int Ch>
    struct lalr_transition
    {
        /*
            This is the core of the algorithm.
            The action is either: shift, reduce, accept, or error.

        */

    };

    template<typename Rule, typename It>
    tree parse(const token_stream<It> &tokens)
    {
        parse_state<It> state;
        state.tokens = tokens;

        // Do the parsing
        using S0 = typeset<rule_position<Rule, 0, 0>>;


        return std::move(state.tree);
    }

    template<typename Lexer, typename Rule, typename It>
    tree parse(It start, It end)
    {
        return parse<Rule>(make_lexer<Lexer,linecounter<It>>().tokenize());
    }
}