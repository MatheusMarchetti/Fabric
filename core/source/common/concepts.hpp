#pragma once

#include <concepts>

namespace fabric
{
	template<typename T, typename U>
	concept is_not_same = !std::is_same_v<T, U>;

	template<typename T, typename U>
	concept is_same = std::is_same_v<T, U>;
}