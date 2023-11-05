#pragma once

#define USE_STL_VECTOR 1
#define USE_STL_SPAN 1
#define USE_STL_UNORDERED_MAP 1
#define USE_STL_OPTIONAL 1
#define USE_STL_REFERENCE_WRAPPER 1

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

#if USE_STL_OPTIONAL
#include <optional>
namespace fabric::utl
{
	template<typename T>
	using optional = typename std::optional<T>;
}
#endif

#if USE_STL_REFERENCE_WRAPPER
#include <functional>
namespace fabric::utl
{
	template<typename T>
	using reference_wrapper = typename std::reference_wrapper<T>;
}
#endif

namespace fabric::utl
{
	template<typename T>
	class ref
	{
	public:
		ref(T* object)
		{
			if (object)
				m_object = *object;
			else
				m_object = {};
		}

		ref(const ref& rhs)
		{
			m_object = rhs.m_object;
		}

		T* operator -> ()
		{
			if (m_object)
				return &m_object.value().get();
			
			return nullptr;
		}

		template<typename U>
		void operator = (U& rhs)
		{
			if (m_object)
				m_object.value() = rhs;
		}

		operator bool()
		{
			return m_object ? true : false;
		}

		template<typename U>
			requires mult_assign<T, U>
		ref<T> operator *= (const U& rhs)
		{
			if (m_object)
				m_object.value().get() *= rhs;
				return *this;

			return nullptr;
		}

		template<typename U>
			requires div_assign<T, U>
		ref<T> operator /= (const U& rhs)
		{
			if (m_object)
				m_object.value().get() /= rhs;
			return *this;

			return nullptr;
		}

		template<typename U>
			requires add_assign<T, U>
		ref<T> operator += (const U& rhs)
		{
			if (m_object)
				m_object.value().get() += rhs;
			return *this;

			return nullptr;
		}

		template<typename U>
			requires sub_assign<T, U>
		ref<T> operator -= (const U& rhs)
		{
			if (m_object)
				m_object.value().get() -= rhs;
			return *this;

			return nullptr;
		}

	private:
		utl::optional<utl::reference_wrapper<T>> m_object;
	};
}

// utl::ref<Transform> transform = ecs::get_component<Transform>();
// transform->position  -- if transform == null then this should be ignored