namespace cellar
{
    // TODO: Turn into T::hash and remove this type completely.
    template<typename T>
    struct hash;

    template<typename...>
    struct typeset;

    template<typename H, typename L, typename R>
    struct type_tree;

    struct empty_tree;

    struct empty_node;

    template<typename H, typename T>
    struct list_node;

    template<int Token, typename Rule>
    struct shift;

    template<int Lookahead, typename Symbol, typename Rule>
    struct reduce;

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
        static const int value = hash_combine<hash<symbol<Rules...>>::value, 127>::value;
    };

    template<int Id, typename...Def, typename...Rules>
    struct hash<symbol<token<Id, Def...>, Rules...>>
    {
        static const int value = hash_combine<hash<symbol<Rules...>>::value, hash<token<Id, Def...>>::value>::value;
    };

    template<int Id, typename...Def, typename...Rules>
    struct hash<symbol<rule<Id, Def...>, Rules...>>
    {
        static const int value = hash_combine<hash<symbol<Rules...>>::value, hash<rule<Id, Def...>>::value>::value;
    };

    template<typename...Def, typename...Rules>
    struct hash<symbol<symbol<Def...>, Rules...>>
    {
        static const int value = hash_combine<hash<symbol<Rules...>>::value, hash<symbol<Def...>>::value>::value;
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

    template<int Lookahead, typename Symbol, typename Rule>
    struct hash<reduce<Lookahead, Symbol, Rule>>
    {
        static const int value = hash_combine<Lookahead, hash_combine<hash<Symbol>::value, hash<Rule>::value>::value>::value;
    };

    template<>
    struct hash<empty_node>
    {
        static const int value = 0;
    };

    template<typename H, typename T>
    struct hash<list_node<H, T>>
    {
        static const int value = hash_combine<hash<H>::value, hash<T>::value>::value;
    };
}
