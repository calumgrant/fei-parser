namespace cellar
{
    template<typename T>
    struct hash;

    template<int Id, typename ... Body>
    struct hash<token<Id, Body...>>
    {
        static const int value = Id;
    };

    // !! Move to hash.hpp
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
        static const int value = 17 * hash<symbol<Rules...>>::value + hash<Rule>::value;
    };

}