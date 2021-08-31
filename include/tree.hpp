#pragma once
#include <vector>
#include "node.hpp"

namespace feiparser
{    
    class tree
    {
    public:
        const node & root() const
        {
            return *(const node*)&data[data.size() - sizeof(node)];
        }

        bool empty() const { return data.empty(); }

        node & shift(int tokenId, int offset, int row, int col, int length);

        node & reduce(int nodeId, int count);

    protected:
        std::vector<char> data;
    };
}
