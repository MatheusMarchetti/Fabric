#include "scene.hpp"
#include "source/utilities/sparse_set.hpp"

#include <iostream>

namespace fabric::ecs
{
    u32 _componentCounter = 0;
    
    namespace
    {
        // Entity storage
        utl::vector<entity_id> m_registry;
        entity_id m_next = id::invalid_id;
        u32 m_free_count = 0;

        // Component storage
        std::unordered_map<component_id, utl::sparse_set> m_component_registry;

        // System storage - temporary
        std::unordered_map<system_id, void(*)()> m_system_registry;
    }

    entity_id create_entity()
    {
        if (m_free_count)
        {
            entity_id id = id::new_generation(m_next);
            id::id_type index = id::index(id);
            
            m_next = m_registry[index];
            m_registry[index] = id;
            
            index = id::index(m_next);

            if (index != id::invalid_index)
            {
                id::id_type generation = id::generation(m_registry[index]);
                m_next = id::new_identifier(index, generation);
            }
            else
                m_next = id::invalid_id;

            m_free_count--;

            return id;
        }

        size_t index = m_registry.size();
        u32 generation = 0;

        entity_id id = m_registry.emplace_back(id::new_identifier(index, generation));

        return id;
    }

    void remove_entity(entity_id id)
    {
        assert(is_alive(id));

        id::id_type index = id::index(id);
        id::id_type generation = id::generation(id);
        id::id_type next_index = id::is_valid(m_next) ? id::index(m_next) : id::invalid_index;

        m_registry[index] = id::new_identifier(next_index, generation);

        m_next = id;
        m_free_count++;
    }

    bool is_alive(entity_id id)
    {
        id::id_type index = id::index(id);
        id::id_type generation = id::generation(id);

        return id::generation(m_registry[index]) == generation;
    }

    system_id register_system(component_id owner, void(*function)())
    {
        m_system_registry[owner] = function;
        return owner;
    }

    void add_dependency(system_id id, component_id dependency)
    {
        std::cout << "System id: " << id << "Dependency id : " << dependency << std::endl;
    }

    void run_systems()
    {
        for (auto& system : m_system_registry)
        {
            system.second();
        }
    }

    bool has_component(entity_id id, component_id component)
    {
        assert(m_component_registry.contains(component));

        if (m_component_registry.contains(component) && m_component_registry[component].has(id))
            return true;

        return false;
    }

    void add_component(component& component)
    {
        if (!m_component_registry.contains(component.id))
            m_component_registry[component.id] = utl::sparse_set::create(component.size);

        m_component_registry[component.id].emplace(component.owner, component.data);
    }

    void remove_component(entity_id id, component_id component)
    {
        assert(has_component(id, component));

        if (has_component(id, component))
        {
            m_component_registry[component].remove(id);
        }
    }

    void* get_component(entity_id id, component_id component)
    {
        assert(has_component(id, component));

        if (has_component(id, component))
            return m_component_registry[component][id];

        return nullptr;
    }
}

