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
    void* linear_block::deallocate(u32 size)
    {
        m_offset -= size;
        m_offset = m_offset < 0 ? 0 : m_offset;

        return nullptr;
    }

    void linear_block::free()
    {
        ::free(m_block);
        m_block = nullptr;
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
}

