#pragma once

#include <concepts>

namespace fabric
{
	template<typename T, typename U>
	concept is_not_same = !std::is_same_v<T, U>;

	template<typename T, typename U>
	concept is_same = std::is_same_v<T, U>;

	template<class U>
	concept enable_if = std::enable_if<U::type>::type;

	template<typename T, typename U>
	concept mult_assign = requires (T a, U b) { {a *= b}; };

	template<typename T, typename U>
	concept div_assign = requires (T a, U b) { {a /= b}; };

	template<typename T, typename U>
	concept add_assign = requires (T a, U b) { {a += b}; };

	template<typename T, typename U>
	concept sub_assign = requires (T a, U b) { {a -= b}; };
}