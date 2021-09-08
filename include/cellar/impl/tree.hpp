#pragma once
#include <vector>
#include "node.hpp"

namespace cellar
{   
    class tree
    {
    public:
        writable_node root()
        {
            return writable_node((node_data*)&data[data.size() - sizeof(node)]);
        }

        node root() const
        {
            return node((const node_data*)&data[data.size() - sizeof(node)]);
        }


        bool empty() const { return data.empty(); }

        writable_node shift(int tokenId, location l, int length)
        {
            std::uint32_t size = sizeof(node_data) + sizeof(location) + length + 1;
            data.resize(data.size() + size);
            auto r = root();
            r.setData({size, std::uint16_t(tokenId), 0});
            r.setLocation(l);
            return r;
        }

        writable_node reduce(int nodeId, int count)
        {
            std::uint32_t size = sizeof(node_data);
            data.resize(data.size() + size);
            auto r = root();
            node child = node((node_data*)(data.data() + data.size() - size - sizeof(node)));
            for(int i=0; i<count; ++i)
            {
                child = child.next();
            }
            // TODO: Typedef some of these
            r.setData({std::uint32_t((const char*)r.p - (const char*)child.p), (std::uint16_t)nodeId, (std::uint16_t)count});
            return r;
        }

        void syntax_error(location l);

        void SyntaxError(const location & l)
        {
            errorLocation = l;
        }

        bool success = false;

    protected:
        location errorLocation;
        std::vector<char> data;
    };
}
