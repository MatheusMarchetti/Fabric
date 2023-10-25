#include "registry.hpp"

namespace fabric::ecs
{
    namespace
    {
        struct component_entry
        {
            u32 component_size = 0;
            id::id_type component_id = id::invalid_id;
            void* component = nullptr;
        };

        struct entity_entry
        {
            u32 total_size = 0;
            u32 component_count = 0;
            utl::vector<component_entry> components;
        };
    }

	id::id_type registry::create_entity()
	{
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
	}

    void registry::remove_entity(id::id_type entity_id)
    {
        id::id_type index = id::index(entity_id);
        id::id_type generation = id::generation(entity_id);
        id::id_type next_index = id::is_valid(m_next) ? id::index(m_next) : id::invalid_index;

        m_entity_registry[index] = id::new_identifier(next_index, generation);

        m_next = entity_id;
        m_free_count++;
    }

    id::id_type& registry::operator[](id::id_type index)
    {
        return m_entity_registry[index];
    }

    sparse_set& registry::get_component_storage(id::id_type component_id)
    {
        assert(component_exists(component_id));
        return m_component_registry[component_id];
    }

    u32 registry::get_component_count(id::id_type component_id)
    {
        return component_exists(component_id) ? m_component_registry[component_id].count() : 0;
    }

    bool registry::component_exists(id::id_type component_id)
    {
        if (m_component_registry.contains(component_id))
            return true;

        return false;
    }

    void registry::register_component(id::id_type component_id, u32 component_size)
    {
        m_component_registry[component_id] = ecs::sparse_set::create(component_size);
    }

    void registry::serialize(FILE* binary_file)
    {
        // get all entities
        utl::vector<entity_entry> entities(m_entity_registry.size());
        u32 total_size = 0;
        // for each component in registry, distribute along their entities
        for (auto& [component_id, component] : m_component_registry)
        {
            u32 component_size = (u32)component.component_size();
            entity* owner_entities = component.dense();
            void* components = component.component();
            
            // for each entity with this component
            for (u32 i = 0; i < component.count(); i++)
            {
                component_entry c_entry
                {
                    .component_size = component_size,
                    .component_id = component_id,
                    .component = (char*)components + i * component_size
                };

                id::id_type eid = owner_entities[i].get_id();
                id::id_type index = id::index(eid);

                entity_entry& e_entry = entities[index];
                e_entry.total_size += component_size;
                e_entry.component_count += 1;
                e_entry.components.push_back(c_entry);

                total_size += component_size;
            }
        }
        
        utl::vector<entity_entry> packed_entities;

        //Tightly pack the entities if there's bubbles inside the registry
        if (m_free_count > 0)
        {
            packed_entities.reserve(entities.size() - m_free_count);

            for (auto& entry : entities)
            {
                if (entry.component_count > 0)
                    packed_entities.push_back(entry);
                else
                    total_size -= entry.total_size;
            }
        }
        else
        {
            packed_entities.swap(entities);
        }

        // Copy packed entities to binary file

        u32 entity_count = (u32)packed_entities.size();  // For validation after reading
        fwrite(&entity_count, sizeof(entity_count), 1, binary_file);
        fwrite(&total_size, sizeof(total_size), 1, binary_file); //Total size needed for components

        for (auto& entity : packed_entities)
        {
            fwrite(&entity.total_size, sizeof(entity.total_size), 1, binary_file); // Total size to be read for entity, for validation
            fwrite(&entity.component_count, sizeof(entity.component_count), 1, binary_file); // Total component count

            for (u32 i = 0; i < entity.component_count; i++)
            {
                component_entry& component = entity.components[i];
                fwrite(&component.component_size, sizeof(component.component_size), 1, binary_file); // Component size to be read
                fwrite(&component.component_id, sizeof(component.component_id), 1, binary_file); // Component ID
                fwrite(component.component, component.component_size, 1, binary_file); // Component data
            }
        }
    }

    bool registry::deserialize(FILE* binary_file)
    {
        u32 entity_count;
        fread(&entity_count, sizeof(entity_count), 1, binary_file);

        u32 total_size;
        fread(&total_size, sizeof(total_size), 1, binary_file);

        u32 offset = 0;

        memory::linear_block block = memory::linear_allocator::request_block(total_size);

        for (u32 i = 0; i < entity_count; i++)
        {
            id::id_type eid = create_entity();
            entity e(eid);

            u32 read = 0;

            u32 entity_size;
            fread(&entity_size, sizeof(entity_size), 1, binary_file);

            u32 component_count;
            fread(&component_count, sizeof(component_count), 1, binary_file);

            for (u32 j = 0; j < component_count; j++)
            {
                u32 component_size;
                fread(&component_size, sizeof(component_size), 1, binary_file);

                id::id_type component_id;
                fread(&component_id, sizeof(component_id), 1, binary_file);

                if (!component_exists(component_id))
                    register_component(component_id, component_size);

                void* data = block.allocate(component_size);

                if (data)
                {
                    fread(data, component_size, 1, binary_file);
                    get_component_storage(component_id).emplace(e, data);

                    read += component_size;
                    offset += component_size;
                }
            }

            if (read != entity_size)
            {
                memory::linear_allocator::return_block(block);
                return false;
            }
        }

        memory::linear_allocator::return_block(block);

        if (offset != total_size)
            return false;

        return true;
    }
}