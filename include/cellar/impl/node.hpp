#pragma once

#include "location.hpp"
#include <string>

namespace cellar
{
    struct node_data
    {
        std::uint32_t size;  // Size of node in bytes, including node_data
        std::uint16_t id, numberOfChildren;
    };

    class children
    {
        int id;
    };

    class node
    {
    public:
        // Constructs an empty node;
        node() : p() {}
        explicit node(const node_data * p) : p(p) {}

        std::string str() const { return std::string(c_str(), c_str() + tokenLength()); }
        
        const char * c_str() const { return (const char*)extraData(); }
        
        location getLocation() const
        {
            return *(cellar::location*)((char*)p - sizeof(cellar::location));
        }

        int id() const { return p->id; }

        node next() const
        {
            return node((node_data*)((char*)p - p->size));
        }

        typedef std::uint16_t size_type;

        // Iteration
        node begin() const { return size()==0 ? end() : first(); }
        node end() const { return next(); }

        node operator[](size_type i) const;

        operator bool() const { return p; }

        // children get(int id);

        // Container of nodes of a given type
        
        bool operator==(node other) const { return p == other.p; }
        bool operator!=(node other) const { return p != other.p; }

        // Gets the first node with a given id, or returns the empty node
        node first() const
        {
            return node((const node_data*)((const char*)p - sizeof(node_data)));
        }

        int size() const { return p ? p->numberOfChildren : 0; }

        // A node is its own iterator
        
        typedef node value_type;
        typedef node iterator;
        
        node operator*() const { return *this; }

        node & operator++()
        {
            *this = next();
            return *this;
        }

        bool isToken() const
        {
            return p && p->numberOfChildren==0;
        }

        bool hasLocation() const
        {
            return isToken() && p->size >= sizeof(node) + sizeof(location);
        }

        bool hasString() const
        {
            return isToken() && p->size > sizeof(node) + sizeof(location);
        }

        const void* extraData() const { return (const char*)p - p->size + sizeof(node_data); }
        
        int tokenLength() const { return isToken() ? p->size - sizeof(node_data) - sizeof(location) - 1 : 0; }

        const node_data * p;
    };

    // Maybe move to impl and away from here

    class writable_node
    {
    public:
        writable_node() : p() {}
        explicit writable_node(node_data * p) : p(p) {}

        operator node() const { return node(p); }

        node_data * p;

        void* extraData() const { return (char*)p - p->size + sizeof(node_data); }

        char * c_str() const { return (char*)extraData(); }

        void setData(node_data data)
        {
            *p = data;
        }

        void setLocation(cellar::location l)
        {
            *(cellar::location*)((char*)p - sizeof(cellar::location)) = l;
        }
    };
}
