#include "memory.hpp"

#include <memory>

namespace fabric::memory
{
    namespace
    {

    }

    linear_block::linear_block(u32 size)
        : m_size(size), m_offset(0)
    {
        m_block = malloc(m_size);
        memset(m_block, -1, m_size);
    }

    void* linear_block::allocate(u32 size)
    {
        if (m_offset + size > m_size)
            return nullptr;

        void* memory = (char*)m_block + m_offset;
        m_offset += size;

        return memory;
    }

    void linear_block::free() const
    {
        ::free(m_block);
    }

    stack_block::stack_block(u32 size)
        : m_size(size), m_offset(0)
    {
        m_block = malloc(m_size);
        memset(m_block, -1, m_size);
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

    void stack_block::deallocate(void* memory)
    {
        header* h = (header*)((char*)memory - sizeof(header));
        u32 size = h->allocation_size;
        m_offset -= size + sizeof(header);
    }

    void stack_block::free() const
    {
        ::free(m_block);
    }

    pool_block::pool_block(u32 count, u32 chunk_size)
        : m_size(count * chunk_size), m_chunk_size(chunk_size)
    {
        m_size = count * (chunk_size + sizeof(header));

        // TODO: Request from dynamic memory allocator
        ::free(m_block);
        m_block = malloc(m_size);

        void* memory = (char*)m_block;
        for (u32 offset = 0; offset < m_size; offset += (chunk_size + sizeof(header)))
        {
            header* h = (header*)((char*)memory + offset);
            
            u32 next_offset = offset + (chunk_size + sizeof(header));

            if (next_offset > m_size)
                h->next = (header*)m_block;
            else
                h->next = (header*)m_block + next_offset;
        }

        m_free = (header*)m_block;

        memset(m_block, -1, m_size);
    }

    void* pool_block::allocate()
    {
        void* memory = (void*)(m_free + sizeof(header));
        m_free = m_free->next;

        return memory;
    }

    void pool_block::deallocate(void* memory)
    {
        header* h = (header*)((char*)memory - sizeof(header));
        h->next = m_free;
    }

    void pool_block::free() const
    {
        ::free(m_block);
    }

    namespace linear_allocator
    {
        linear_block request_block(u32 block_size)
        {
            return linear_block(block_size);
        }

        void return_block(const linear_block& block)
        {
            block.free();
        }
    }

    namespace stack_allocator
    {
        stack_block request_block(u32 block_size)
        {
            return stack_block(block_size);
        }

        void return_block(const stack_block& block)
        {
            block.free();
        }
    }

    namespace pool_allocator
    {
        pool_block request_block(u32 count, u32 chunk_size)
        {
            return pool_block(count, chunk_size);
        }
        void return_block(const pool_block& block)
        {
            block.free();
        }
    }

    namespace raw_allocator
    {
        void* request_raw(u32 size)
        {
            return malloc(size);
        }

        void return_raw(void* block)
        {
            ::free(block);
            block = nullptr;
        }
    }
}

