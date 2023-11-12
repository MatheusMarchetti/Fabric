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

	template<typename T>
	concept has_initialize = requires (T t) { {t.initialize()} -> std::same_as<bool>; };

	template<typename T, typename U>
	concept has_mult = requires (T a, U b) { {a * b} -> std::same_as<T>; };

	template<typename T, typename U>
	concept has_div = requires (T a, U b) { {a / b} -> std::same_as<T>; };

	template<typename T, typename U>
	concept has_add = requires (T a, U b) { {a + b} -> std::same_as<T>; };

	template<typename T, typename U>
	concept has_sub = requires (T a, U b) { {a - b} -> std::same_as<T>; };

	template<typename T, typename U>
	concept has_mult_assign = requires (T a, U b) { {a *= b}; };

	template<typename T, typename U>
	concept has_div_assign = requires (T a, U b) { {a /= b}; };

	template<typename T, typename U>
	concept has_add_assign = requires (T a, U b) { {a += b}; };

	template<typename T, typename U>
	concept has_sub_assign = requires (T a, U b) { {a -= b}; };
}