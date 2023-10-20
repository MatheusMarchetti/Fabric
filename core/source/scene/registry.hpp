#pragma once

#include "common.hpp"
#include "sparse_set.hpp"

namespace fabric::ecs
{
	class registry
	{
	public:
		id::id_type create_entity()
		{
            if (m_free_count)
            {
                id::id_type id = id::new_generation(m_next);
                id::id_type index = id::index(id);

                m_next = m_entity_registry[index];
                m_entity_registry[index] = id;

                index = id::index(m_next);

                if (index != id::invalid_index)
                {
                    id::id_type generation = id::generation(m_entity_registry[index]);
                    m_next = id::new_identifier(index, generation);
                }
                else
                    m_next = id::invalid_id;

                m_free_count--;

                return id;
            }

            size_t index = m_entity_registry.size();
            u32 generation = 0;

            id::id_type id = m_entity_registry.emplace_back(id::new_identifier(index, generation));

            return id;
		}

        void remove_entity(id::id_type entity_id)
        {
            id::id_type index = id::index(entity_id);
            id::id_type generation = id::generation(entity_id);
            id::id_type next_index = id::is_valid(m_next) ? id::index(m_next) : id::invalid_index;

            m_entity_registry[index] = id::new_identifier(next_index, generation);

            m_next = entity_id;
            m_free_count++;
        }

		id::id_type& operator[](id::id_type index)
		{
			return m_entity_registry[index];
		}

        sparse_set& get_component_storage(id::id_type component_id)
        {
            assert(component_exists(component_id));
            return m_component_registry[component_id];
        }

        size_t get_component_count(id::id_type component_id)
        {
            return component_exists(component_id) ? m_component_registry[component_id].count() : 0;
        }

        bool component_exists(id::id_type component_id)
        {
            if (m_component_registry.contains(component_id))
                return true;

            return false;
        }

        void register_component(id::id_type component_id, size_t component_size)
        {
            m_component_registry[component_id] = ecs::sparse_set::create(component_size);
        }

	private:
		utl::vector<id::id_type> m_entity_registry;
        std::unordered_map<id::id_type, sparse_set> m_component_registry;

		id::id_type m_next = id::invalid_id;
		u32 m_free_count = 0;
	};
}