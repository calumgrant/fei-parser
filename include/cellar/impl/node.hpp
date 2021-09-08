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

    /*
        A reference to a node in the parse tree.
        This is a lightweight object, so there is no need to create a reference to it.
        (Prefer `node` to `const node&` for example.)

        `node` does not store any data itself, but simply points into the `tree`.

        A node is only valid for the duration of the underlying `tree` object,
        provided that the underlying object is not modified or destroyed. Use of a node
        on a destroyed tree is undefined, and will probably crash.

        `node` are const, and can be used in a thread-safe manner. `node` is nothrow,
        except for `str()` which could fail with `bad_alloc`.

        There are a few methods which can have undefined behaviour if abused, so beware.

        For efficiency, parse trees are stored in a compact form on a stack of chars,
        (see `tree::data`) so the code is not so readable.
    */
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

        node operator[](size_type i) const
        {
            if(i>=size()) return *this;
            node n = begin();
            for(++i; i<size(); ++i)
                ++n;
            return n;
        }

        operator bool() const { return p; }

        // children get(int id);

        template<typename Fn>
        void visit(Fn fn) const
        {
            for(auto i : *this)
            {
                if(i.id() == Hidden)
                    i.visit(fn);
                else
                    fn(i);
            }
        }

        // TODO: Visit a type

        template<typename Fn>
        void visitRecursive(Fn fn) const
        {
            if(id() != Hidden)
                fn(*this);
            for(auto i : *this)
            {
                i.visitRecursive(fn);
            }
        }

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
