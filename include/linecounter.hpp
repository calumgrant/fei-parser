#pragma once

#include <iterator>

namespace feiparser
{
    template<typename It>
    class linecounter
    {
    public:
        int row=1;
        int col=1;

        linecounter(It i) : it(i)
        {
        }

        linecounter & operator++()
        {
            if(*it++ == '\n')
                col=1, row++;
            else
                col++;
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
