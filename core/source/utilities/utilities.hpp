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
		ref() = delete;
		ref(T* object);

		ref(const ref& rhs);
		ref(const ref&& rhs);

		~ref();

		T* operator -> ();

		template<typename U>
		void operator = (U& rhs);

		operator bool();

		// If T supports arithmetic operations, ref<T> also supports them
		template<typename U> requires has_mult<T, U>
		T operator * (const U& rhs);

		template<typename U> requires has_div<T, U>
		T operator / (const U& rhs);

		template<typename U> requires has_add<T, U>
		T operator + (const U& rhs);

		template<typename U> requires has_sub<T, U>
		T operator - (const U& rhs);

		template<typename U> requires has_mult_assign<T, U>
		ref<T> operator *= (const U& rhs);

		template<typename U> requires has_div_assign<T, U>
		ref<T> operator /= (const U& rhs);

		template<typename U> requires has_add_assign<T, U>
		ref<T> operator += (const U& rhs);

		template<typename U> requires has_sub_assign<T, U>
		ref<T> operator -= (const U& rhs);

	private:
		utl::optional<utl::reference_wrapper<T>> m_object;
	};

	template<typename T>
	inline ref<T>::ref(T* object)
	{
		if (object)
			m_object = *object;
		else
			m_object = {};
	}

	template<typename T>
	inline ref<T>::ref(const ref& rhs)
	{
		m_object = rhs.m_object;
	}

	template<typename T>
	inline ref<T>::ref(const ref&& rhs)
	{
		m_object = std::move(rhs.m_object);
		rhs.m_object = {};
	}

	template<typename T>
	inline ref<T>::~ref()
	{
		m_object = {};
	}

	template<typename T>
	inline T* ref<T>::operator -> ()
	{
		if (m_object)
			return &m_object.value().get();

		return nullptr;
	}

	template<typename T>
	template<typename U>
	inline void ref<T>::operator = (U& rhs)
	{
		if (m_object)
			m_object.value() = rhs;
	}

	template<typename T>
	inline ref<T>::operator bool()
	{
		return m_object ? true : false;
	}
	template<typename T>
	template<typename U> requires has_mult_assign<T, U>
	inline ref<T> ref<T>::operator *= (const U& rhs)
	{
		if (m_object)
			m_object.value().get() *= rhs;
		return *this;

		return nullptr;
	}

	template<typename T>
	template<typename U> requires has_div_assign<T, U>
	inline ref<T> ref<T>::operator /= (const U& rhs)
	{
		if (m_object)
			m_object.value().get() /= rhs;
		return *this;

		return nullptr;
	}

	template<typename T>
	template<typename U> requires has_add_assign<T, U>
	ref<T> ref<T>::operator += (const U& rhs)
	{
		if (m_object)
			m_object.value().get() += rhs;
		return *this;

		return nullptr;
	}

	template<typename T>
	template<typename U> requires has_sub_assign<T, U>
	ref<T> ref<T>::operator -= (const U& rhs)
	{
		if (m_object)
			m_object.value().get() -= rhs;
		return *this;

		return nullptr;
	}

	template<typename T>
	template<typename U> requires has_mult<T, U>
	inline T ref<T>::operator * (const U& rhs)
	{
		if (m_object)
		{
			T result;
			result = m_object.value().get() * rhs;

			return result;
		}

		return T();
	}

	template<typename T>
	template<typename U> requires has_div<T, U>
	inline T ref<T>::operator / (const U& rhs)
	{
		if (m_object)
		{
			T result;
			result = m_object.value().get() / rhs;

			return result;
		}

		return T();
	}

	template<typename T>
	template<typename U> requires has_add<T, U>
	inline T ref<T>::operator + (const U& rhs)
	{
		if (m_object)
		{
			T result;
			result = m_object.value().get() + rhs;

			return result;
		}

		return T();
	}

	template<typename T>
	template<typename U> requires has_sub<T, U>
	inline T ref<T>::operator - (const U& rhs)
	{
		if (m_object)
		{
			T result;
			result = m_object.value().get() - rhs;

			return result;
		}

		return T();
	}
}