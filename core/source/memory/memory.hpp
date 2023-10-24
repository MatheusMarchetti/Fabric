#pragma once

namespace fabric::memory
{
	namespace pool_allocator
	{
		void* allocate(size_t size);
		void deallocate(void* block);
	}

	namespace linear_allocator
	{
		void* allocate(size_t size);
		void deallocate(void* block);
	}
}