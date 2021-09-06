#pragma once

#include "location.hpp"

namespace cellar
{
    struct node_data;
    class children;

    class node
    {
    public:
        // Constructs an empty node;
        node();
        node(const node_data * p);

        std::string str() const;
        const char * c_str() const;
        cellar::location location() const;

        int length() const;

        int id() const;
        node first() const;
        node next() const;

        typedef std::uint16_t size_type;

        struct iterator
        {
            typedef node value_type;

            node operator*() const;
            node operator->() const;
            iterator & operator++();
        };

        // Iteration
        iterator begin() const;
        iterator end() const;

        node operator[](size_type i) const;

        operator bool() const { return p; }

        children get(int id);

        // Container of nodes of a given type


        // Gets the first node with a given id, or returns the empty node
        node first(int id) const;
    private:
        const node_data * p;
    };

    class children
    {
        int id;
        node a, b;
    };

    /*
        A node in the parse tree.

        This is a compact and memory-efficient representation designed basically for speed.
        The nodes are stored in a vector<char>, and are navigated using pointer offsets.
        
        This has advantages:
        1. It supports the shift and reduce operations but just pushing a new node at the end of the vector 
        2. It removes the need for memory allocation, except to extend a vector
        3. Memory locality: adjacent nodes are stored adjacently in memory.
        4. Memory safety: The vector cleans up the contents at the end.
     */ 
    class node2
    {
    public:

        int id() const { return nodeId; }
        int size() const { return numberOfChildren; }

        const char * c_str();

        bool leaf() const { return size()==0; }

        class iterator
        {
        public:
            iterator(const node2 *n) : current(n) {}

            iterator & operator++()
            {
                current = &current->previous();
                return *this;
            }

            bool operator!=(iterator other) const { return current != other.current; }

            const node2 & operator*() const { return *current; }

        private:
            const node2 * current;
        };

        iterator begin() const
        {
            return &first();
        }

        iterator end() const
        {
            return &previous();
        }

        const node2 & previous() const
        {
            return *(this - sizeInNodes);
        }

        const node2 & first() const
        {
            return *(this-1);
        }

        const node2 & operator[](int child) const
        {
            for(const auto & i : *this)
            {
                if(child-- == 0) return i;
            }
            // Undefined / return something
            return *this;
        }

        const void * data() const
        {
            return (const void*)(this + 1 - sizeInNodes);
        }

    private:
        std::uint32_t sizeInNodes;  // The size of the node, including this node data.
        std::uint16_t nodeId;
        std::uint16_t numberOfChildren;
    };
}
