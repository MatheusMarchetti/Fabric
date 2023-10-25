#pragma once

#include "../common/common_headers.hpp"

namespace fabric::memory
{
	class linear_block
	{
	public:
		linear_block() = default;
		linear_block(u32 size);

		void* allocate(u32 size);

		// temporary
		void free() const;

	private:
		void* m_block = nullptr;
		u32 m_size = 0;
		u32 m_offset = 0;
	};

	class stack_block
	{
	public:
		stack_block() = default;
		stack_block(u32 size);
		void* allocate(u32 size);
		void deallocate(void* memory);

		// temporary
		void free() const;

	private:
		struct header
		{
			u32 allocation_size;
		};

	private:
		void* m_block = nullptr;
		u32 m_size = 0;
		u32 m_offset = 0;
	};

	class pool_block
	{
	public:
		pool_block() = default;
		pool_block(u32 count, u32 chunk_size);
		void* allocate();
		void deallocate(void* memory);

		// temporary
		void free() const;

	private:
		struct header
		{
			header* next;
		};

	private:
		void* m_block = nullptr;
		header* m_free = nullptr;
		u32 m_size = 0;
		u32 m_chunk_size = 0;
	};

	namespace linear_allocator
	{
		linear_block request_block(u32 block_size);
		void return_block(const linear_block& block);
	}

	namespace stack_allocator
	{
		stack_block request_block(u32 block_size);
		void return_block(const stack_block& block);
	}

	namespace pool_allocator
	{
		pool_block request_block(u32 count, u32 chunk_size);
		void return_block(const pool_block& block);
	}

	namespace raw_allocator
	{
		void* request_raw(u32 size);
		void return_raw(void* block);
	}
}