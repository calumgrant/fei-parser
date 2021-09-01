#pragma once

#include "location.hpp"
#include <iterator>

namespace feiparser
{
    template<typename It>
    class linecounter
    {
    public:
        location location;

        linecounter() {}

        linecounter(It i) : it(i)
        {
            location.row = 1;
            location.col = 1;
            location.offset = 1;
        }

        linecounter & operator++()
        {
            if(*it++ == '\n')
                location.col=1, location.row++;
            else
                location.col++;
            ++location.offset;
            return *this;
        }

        typedef typename std::iterator_traits<It>::value_type value_type;
        value_type operator*() const { return *it; }
        value_type operator->() const { return *it; }

        bool operator==(const linecounter & other) const
        {
            return it == other.it;
        }

        bool operator!=(const linecounter & other) const
        {
            return it != other.it;
        }


        // TODO: Other iterator stuff
    private:
        It it;
    };
}
