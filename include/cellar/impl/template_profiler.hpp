/*
    A template-profiler used to count the number of template instantiations.
    This is necessary for debugging and optimizing the library but is not
    generally needed otherwise.

    Usage:
*/

namespace cellar
{
    template<typename Tag>
    int &static_count()
    {
        static int number_of_instances = 0;
        return number_of_instances;
    };

    template<typename Tag>
    int increment(bool & created)
    {
        created = true;
        return ++static_count<Tag>();
    };

    template<typename Type>
    struct profile_traits
    {
        // TODO
    };

    template<typename...Items>
    struct profile_types;

    template<>
    struct profile_types<>
    {
        static void collect_stats()
        {
        }
    };

    template<typename Type, typename Tag = typename Type::profile_tag>
    void profile_template()
    {
        bool created = false;
        static int value = increment<Tag>(created);
        if(created)
        {
            // First initialization
            Type::profile_types::collect_stats();
        }
    }

    template<typename Item, typename...Items>
    struct profile_types<Item, Items...>
    {
        static void collect_stats()
        {
            profile_template<Item>();
            profile_types<Items...>::collect_stats();
        }
    };    
}
