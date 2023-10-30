#pragma once

#define USE_STL_VECTOR 1
#define USE_STL_SPAN 1
#define USE_STL_UNORDERED_MAP 1

#if USE_STL_VECTOR
#include <vector>
namespace fabric::utl
{
	template<typename T>
	using vector = typename std::vector<T>;

	template<typename T>
	void erase_unordered(std::vector<T>& v, size_t index)
	{
		if (v.size() > 1)
		{
			std::iter_swap(v.begin() + index, v.end() - 1);
			v.pop_back();
		}
		else
		{
			v.clear();
		}
	}
}
#endif

#if USE_STL_SPAN
#include <span>
namespace fabric::utl
{
	template<typename T>
	using span = std::span<T>;
}
#endif

#if USE_STL_UNORDERED_MAP
#include <unordered_map>
namespace fabric::utl
{
	template<typename Key, typename Value>
	using unordered_map = typename std::unordered_map<Key, Value>;
}
#endif