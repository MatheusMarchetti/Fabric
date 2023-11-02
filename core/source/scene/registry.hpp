#pragma once

#include "common.hpp"
#include "sparse_set.hpp"

namespace fabric::ecs
{
	class registry
	{
	public:
        id::id_type create_entity();

        void remove_entity(id::id_type entity_id);

        id::id_type& operator[](id::id_type index);

        sparse_set& get_component_storage(id::id_type component_id);

        u32 get_component_count(id::id_type component_id);

        bool component_exists(id::id_type component_id);

		void register_component(id::id_type component_id, u32 component_size);

		void serialize(FILE* binary_file);

		bool deserialize(FILE* binary_file);

		void clear();

	private:
		utl::vector<id::id_type> m_entity_registry;
        utl::unordered_map<id::id_type, sparse_set> m_component_registry;

		id::id_type m_next = id::invalid_id;
		u32 m_free_count = 0;
	};
}