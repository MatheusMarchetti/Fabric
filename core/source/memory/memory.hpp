#pragma once

#include "../common/common_headers.hpp"

namespace fabric::memory
{
	class linear_block
	{
	public:
		linear_block(void* memory, u32 size);
		void* allocate(u32 size);

		// temporary
		void free();

	private:
		void* m_block;
		u32 m_size;
		u32 m_offset;
	};

	class stack_block
	{
	public:
		stack_block(void* memory, u32 size);
		void* allocate(u32 size);
		void* deallocate(void* memory);

		// temporary
		void free();

	private:
		struct header
		{
			u32 allocation_size;
		};

	private:
		void* m_block;
		u32 m_size;
		u32 m_offset;
	};

	namespace linear_allocator
	{
		linear_block request_block(u32 block_size);
		void return_block(linear_block& block);
	}

	namespace stack_allocator
	{
		stack_block request_block(u32 block_size);
		void return_block(stack_block& block);
	}

	namespace pool_allocator
	{
		void* allocate(size_t size);
		void deallocate(void* block);
	}
}