#pragma once

namespace feiparser
{
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
    class node
    {
    public:

        int id() const { return nodeId; }
        int size() const { return numberOfChildren; }

        const char * c_str();

        bool leaf() const { return size()==0; }

        class iterator
        {
        public:
            iterator(const node *n) : current(n) {}

            iterator & operator++()
            {
                current = &current->previous();
                return *this;
            }

            bool operator!=(iterator other) const { return current != other.current; }

            const node & operator*() const { return *current; }

        private:
            const node * current;
        };

        iterator begin() const
        {
            return &first();
        }

        iterator end() const
        {
            return &previous();
        }

        const node & previous() const
        {
            return *(this - sizeInNodes);
        }

        const node & first() const
        {
            return *(this-1);
        }

        const node & operator[](int child) const
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
