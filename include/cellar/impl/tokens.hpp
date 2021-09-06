
namespace feiparser
{
    namespace impl
    {
        template<typename Symbol, typename Visited, bool Recursive = typeset_contains<Symbol, Visited>::value>
        struct tokens
        {
            using type = typename tokens<typename Symbol::rules, Visited>::type;
        };
    }

    template<typename Symbol>
    struct tokens
    {
        using type = typename impl::tokens<Symbol>::type;
    };
}
