namespace cellar
{
    template<typename T>
    struct hash;

    template<typename...>
    struct typeset;

    template<typename H, typename L, typename R>
    struct type_tree;

    struct empty_tree;

    template<int Token, typename Rule>
    struct shift;

    template<int Id, typename ... Body>
    struct hash<token<Id, Body...>>
    {
        static const int value = Id;
    };

    // static constexpr int hash_combine(int h1, int h2) { return h1 + 17*h2; }

    template<int H1, int H2>
    struct hash_combine
    {
        static const int value = H1 + 17 * (H2&0x00ffffff);
    };

    template<typename T>
    struct hash
    {
        static const int value = hash<typename T::rules>::value;
    };

    template<>
    struct hash<symbol<>>
    {
        static const int value = 15;
    };

    template<int Id, typename...Body>
    struct hash<rule<Id, Body...>>
    {
        static const int value = Id;
    };

    template<typename Rule, typename...Rules>
    struct hash<symbol<Rule, Rules...>>
    {
        static const int value = hash_combine<hash<symbol<Rules...>>::value, hash<Rule>::value>::value;
    };

    template<typename Symbol, typename Rule, int Pos, int Lookahead>
    struct hash<rule_position<Symbol, Rule, Pos, Lookahead>>
    {
        static const int value = hash_combine<hash<Symbol>::value, hash_combine<hash<Rule>::value, hash_combine<Pos, Lookahead>::value>::value>::value;
    };

    // Hash a typeset -- deleteme eventually.
    template<>
    struct hash<typeset<>>
    {
        static const int value = 7;
    };

    template<typename Item, typename...Items>
    struct hash<typeset<Item, Items...>>
    {
        static const int value = hash_combine<hash<Item>::value, hash<typeset<Items...>>::value>::value;
    };

    template<typename H, typename L, typename R>
    struct hash<type_tree<H,L,R>>
    {
        static const int value = hash_combine<
            hash<H>::value, 
            hash_combine<hash<L>::value, hash<R>::value>::value>::value;
    };

    template<>
    struct hash<empty_tree>
    {
        static const int value = 0;
    };

    template<int Token, typename Rule>
    struct hash<shift<Token, Rule>>
    {
        static const int value = hash_combine<Token, hash<Rule>::value>::value;
    };
}
