#include "memory.hpp"

#include <memory>

namespace fabric::memory
{
    namespace
    {

    }

    linear_block::linear_block(void* memory, u32 size)
        : m_block(memory), m_size(size), m_offset(0)
    {
        memset(m_block, 0, m_size);
    }

    void* linear_block::allocate(u32 size)
    {
        if (m_offset + size > m_size)
            return nullptr;

        void* memory = (char*)m_block + m_offset;
        m_offset += size;

        return memory;
    }

    void linear_block::free()
    {
        ::free(m_block);
        m_block = nullptr;
    }

    stack_block::stack_block(void* memory, u32 size)
        : m_block(memory), m_size(size), m_offset(0)
    {
        memset(m_block, 0, m_size);
    }

    void* stack_block::allocate(u32 size)
    {
        if (m_offset + size + sizeof(header) > m_size)
            return nullptr;
        
        header h;
        h.allocation_size = size;

        void* memory = (char*)m_block + m_offset;
        m_offset += sizeof(header);
        *(header*)memory = h;
        memory = (char*)memory + m_offset;
        m_offset += size;

        return memory;
    }

    void* stack_block::deallocate(void* memory)
    {
        header* h = (header*)((char*)memory - sizeof(header));
        u32 size = h->allocation_size;
        m_offset -= size + sizeof(header);

        return (char*)m_block + m_offset;
    }

    void stack_block::free()
    {
        ::free(m_block);
        m_block = nullptr;
    }

    namespace linear_allocator
    {
        linear_block request_block(u32 block_size)
        {
            return linear_block(malloc(block_size), block_size);
        }

        void return_block(linear_block& block)
        {
            block.free();
        }
    }

    namespace stack_allocator
    {
        stack_block request_block(u32 block_size)
        {
            return stack_block(malloc(block_size), block_size);
        }

        void return_block(stack_block& block)
        {
            block.free();
        }
    }

    namespace pool_allocator
    {
        void* allocate(size_t size)
        {
            return malloc(size);
        }

        void deallocate(void* block)
        {
            ::free(block);
            block = nullptr;
        }
    }
}
