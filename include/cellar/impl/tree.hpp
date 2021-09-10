#pragma once
#include <vector>
#include "node.hpp"

namespace cellar
{   
    class tree
    {
    public:
        // Remove these methods into writable_tree ??
        writable_node writableRoot()
        {
            return writable_node((node_data*)&data[data.size() - sizeof(node)]);
        }

        node root() const
        {
            return node((const node_data*)&data[data.size() - sizeof(node)]);
        }

        bool empty() const { return data.empty(); }

        operator bool() const { return success; }

        writable_node shift(int tokenId, location l, int length)
        {
            std::uint32_t size = sizeof(node_data) + sizeof(location) + length + 1;
            data.resize(data.size() + size);
            auto r = writableRoot();
            r.setData({size, std::int16_t(tokenId), 0});
            r.setLocation(l);
            return r;
        }

        void reduce(int nodeId, int count)
        {
            if(nodeId == Removed)
            {
                // Remove `count` nodes from the top
                auto n = root();
                auto size = 0;
                for(int i=0; i<count; ++i)
                {
                    size += n.p->size;
                    ++n;
                }

                data.resize(data.size() - size + sizeof(node_data));
                auto r = writableRoot();
                r.setData({sizeof(node_data), (std::int16_t)Hidden, 0});

                return;
            }

            if(count==1 && root().id() == nodeId)
            {
                // Avoid creating nodes that just wrap a node of the same type
                return; // writableRoot();
            }
            std::uint32_t size = sizeof(node_data);
            data.resize(data.size() + size);
            auto r = writableRoot();
            node child = node((node_data*)(data.data() + data.size() - size - sizeof(node)));
            int addedMembers = 0;
            for(int i=0; i<count; ++i)
            {
                auto oldChild = child;
                child = child.next();
                if(oldChild.id() == Hidden)
                {
                    addedMembers += oldChild.size()-1;
                    writable_node n = oldChild;
                    n.setData({sizeof(node_data), Hidden, 0});
                }
            }
            // TODO: Typedef some of these
            r.setData({std::uint32_t((const char*)r.p - (const char*)child.p), (std::int16_t)nodeId, (std::uint16_t)(count+addedMembers)});
            // return r;
        }

        void syntax_error(location l);

        void SyntaxError(const location & l)
        {
            errorLocation = l;
        }

        void clear()
        {
            data.resize(0);  // Note: keep unreserved size
            success = false;
        }

        location getError() const { return errorLocation; }

        location errorLocation;
        bool success = false;

    protected:
        std::vector<char> data;
    };
}
