#pragma once

#include "scene.hpp"
#include "source/common/common_headers.hpp"
#include "source/common/identifier.hpp"

namespace fabric::ecs
{
	class sparse_set
	{
	public:
		sparse_set() = default;

		sparse_set(u32 component_size, u32 size = 1024)
			: m_component_size(component_size)
		{
			resize(size);
		}

		sparse_set(const sparse_set& other)
		{
			m_component_size = other.m_component_size;
			
			resize(other.m_size);

			memcpy_s(m_sparse, m_size * sizeof(id::id_type), other.m_sparse, m_size * sizeof(id::id_type));
			memcpy_s(m_dense, m_size * sizeof(entity), other.m_dense, m_size * sizeof(entity));
			memcpy_s(m_component, m_size * m_component_size, other.m_component, m_size * m_component_size);
		}

		sparse_set& operator=(const sparse_set& other)
		{
			m_component_size = other.m_component_size;

			resize(other.m_size);

			memcpy_s(m_sparse, m_size * sizeof(id::id_type), other.m_sparse, m_size * sizeof(id::id_type));
			memcpy_s(m_dense, m_size * sizeof(entity), other.m_dense, m_size * sizeof(entity));
			memcpy_s(m_component, m_size * m_component_size, other.m_component, m_size * m_component_size);

			return *this;
		}

		~sparse_set()
		{
			m_component_size = 0;
			m_size = 0;

			memory::raw_allocator::return_raw(m_sparse);
			memory::raw_allocator::return_raw(m_dense);
			memory::raw_allocator::return_raw(m_component);
		}

		static sparse_set create(u32 component_size, u32 size = 1024)
		{
			return sparse_set(component_size, size);
		}

		void emplace(entity e, void* data)
		{
			id::id_type id = e.get_id();

			assert(id::is_valid(id));

			if (id::is_valid(id))
			{
				id::id_type index = id::index(id);

				if (m_size > index)
				{
					size_t next_component = (m_next_component - (char*)m_component) / m_component_size;

					if (id::is_valid(m_sparse[index]))
					{
						next_component = m_sparse[index];
					}

					m_sparse[index] = next_component;
					m_dense[next_component] = e;

					if(data)
						memcpy_s((char*)m_component + next_component * m_component_size, m_component_size, data, m_component_size);

					m_next_component = (char*)m_component + (next_component + 1) * m_component_size;
				}
				else
				{
					resize(2 * m_size);
					emplace(e, data);
				}
			}
		}

		bool has(id::id_type id)
		{
			assert(id::is_valid(id));
			if (id::is_valid(id))
			{
				id::id_type index = id::index(id);

				if (m_size > index)
				{
					return m_sparse[index] != id::invalid_id;
				}
			}

			return false;
		}

		bool has(entity e)
		{
			id::id_type id = e.get_id();

			return has(id);
		}

		u32 size() { return m_size; }
		u32 count() { return (u32)(m_next_component - (char*)m_component) / m_component_size; }
		bool empty() { return m_dense == nullptr; }
		u32 component_size() { return m_component_size; }

		entity* dense() { return m_dense; }
		void* component() { return m_component; }

		void resize(u32 size)
		{
			id::id_type* sparse = (id::id_type*)memory::raw_allocator::request_raw(size * sizeof(id::id_type));
			entity* dense = (entity*)memory::raw_allocator::request_raw(size * sizeof(entity));
			void* component = memory::raw_allocator::request_raw(size * m_component_size);

			if(sparse)
				memset(sparse, -1, size * sizeof(id::id_type));

			if(dense)
				memset(dense, -1, size * sizeof(entity));

			if(component)
				memset(component, 0, size * m_component_size);

			if(m_sparse)
			{
				assert(m_dense && m_component);

				memcpy_s(sparse, size * sizeof(id::id_type), m_sparse, m_size);
				memcpy_s(dense, size * sizeof(entity), m_dense, m_size);
				memcpy_s(component, size * m_component_size, m_sparse, m_size);

				memory::raw_allocator::return_raw(m_sparse);
				memory::raw_allocator::return_raw(m_dense);
				memory::raw_allocator::return_raw(m_component);
			}

			m_next_component = (char*)component + (m_size * m_component_size);
			m_sparse = sparse;
			m_dense = dense;
			m_component = component;
			m_size = size;
		}

		void* operator[](id::id_type id)
		{
			id::id_type index = m_sparse[id::index(id)];

			if (id::is_valid(index))
				return (char*)(char*)m_component + index * m_component_size;

			return nullptr;
		}

		void remove(entity e)
		{
			id::id_type id = e.get_id();

			assert(id::is_valid(id));

			if (id::is_valid(id))
			{
				id::id_type index = m_sparse[id::index(id)];
				size_t last = ((m_next_component - (char*)m_component) / m_component_size);
				
				if (id::is_valid(index) && last > 0)
				{
					last = last - 1;
					m_next_component -= m_component_size;

					m_dense[index] = m_dense[last];
					m_dense[last] = e;
				
					char* temp[256];	// 256 bytes per component SHOULD be enough...

					memcpy_s(temp, m_component_size, (char*)m_component + index * m_component_size, m_component_size);
					memcpy_s((char*)m_component + index * m_component_size, m_component_size, (char*)m_component + last * m_component_size, m_component_size);
					memcpy_s((char*)m_component + last * m_component_size, m_component_size, temp, m_component_size);

					m_sparse[id::index(m_dense[last].get_id())] = index;
					m_sparse[id::index(id)] = last;
				}
			}
		}

	private:
		u32 m_component_size = 0;
		u32 m_size = 0;
		char* m_next_component = nullptr;
		id::id_type* m_sparse = nullptr;
		entity* m_dense = nullptr;
		void* m_component = nullptr;
	};
}