#pragma once

#include "common.hpp"
#include "sparse_set.hpp"

namespace fabric::ecs
{
	class registry
	{
	public:
		using system_proc = void(*)();

        id::id_type create_entity();

        void remove_entity(id::id_type entity_id);

        id::id_type& operator[](id::id_type index);

		void assign_component_to_entity(id::id_type entity_id, id::id_type component_id);

		void remove_component_from_entity(id::id_type entity_id, id::id_type component_id);

        sparse_set& get_component_storage(id::id_type component_id);

        u32 get_component_count(id::id_type component_id);

        bool component_exists(id::id_type component_id);

		void register_component(id::id_type component_id, u32 component_size);

		void register_system(id::id_type component_id, system_proc func);

		system_proc get_system_proc(id::id_type component_id);

		void serialize(FILE* binary_file);

		bool deserialize(FILE* binary_file);

		void clear();

	private:
		utl::vector<id::id_type> m_entity_registry;
		utl::vector<utl::set<id::id_type>> m_archetypes;
        utl::unordered_map<id::id_type, sparse_set> m_component_registry;
		utl::unordered_map<id::id_type, system_proc> m_system_registry;

		id::id_type m_next = id::invalid_id;
		u32 m_free_count = 0;
	};
}