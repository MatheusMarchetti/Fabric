#include "scene.hpp"
#include "sparse_set.hpp"

#include <iostream>

namespace fabric::ecs
{
    u32 _componentCounter = 0;
    
    namespace
    {
        // Entity storage
        utl::vector<id::id_type> m_registry;
        id::id_type m_next = id::invalid_id;
        u32 m_free_count = 0;

        // Component storage
        std::unordered_map<id::id_type, sparse_set> m_component_registry;

        // System storage - temporary
        std::unordered_map<id::id_type, void(*)()> m_system_registry;
    }

    entity create_entity()
    {
        if (m_free_count)
        {
            id::id_type id = id::new_generation(m_next);
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

            return entity(id);
        }

        size_t index = m_registry.size();
        u32 generation = 0;

        id::id_type id = m_registry.emplace_back(id::new_identifier(index, generation));

        return entity(id);
    }

    void remove_entity(entity e)
    {
        assert(is_alive(e));
        
        id::id_type id = e.get_id();

        id::id_type index = id::index(id);
        id::id_type generation = id::generation(id);
        id::id_type next_index = id::is_valid(m_next) ? id::index(m_next) : id::invalid_index;

        m_registry[index] = id::new_identifier(next_index, generation);

        m_next = id;
        m_free_count++;
    }

    bool is_alive(entity e)
    {
        id::id_type id = e.get_id();
        id::id_type index = id::index(id);
        id::id_type generation = id::generation(id);

        return id::generation(m_registry[index]) == generation;
    }

    id::id_type register_system(id::id_type owner, void(*function)())
    {
        m_system_registry[owner] = function;
        return owner;
    }

    void add_dependency(id::id_type system_id, id::id_type dependency)
    {
        std::cout << "System id: " << system_id << "Dependency id : " << dependency << std::endl;
    }

    void run_systems()
    {
        for (auto& system : m_system_registry)
        {
            system.second();
        }
    }

    std::span<entity> get_entities_with(id::id_type component_id)
    {
        size_t size = m_component_registry.contains(component_id) ? m_component_registry[component_id].count() : 0;
        entity* entities = m_component_registry[component_id].dense();

        return std::span<entity>(entities, size);
    }

    bool has_component(entity e, id::id_type component)
    {
        assert(m_component_registry.contains(component));

        if (m_component_registry.contains(component) && m_component_registry[component].has(e))
            return true;

        return false;
    }

    void add_component(component& component)
    {
        if (!m_component_registry.contains(component.id))
            m_component_registry[component.id] = ecs::sparse_set::create(component.size);

        m_component_registry[component.id].emplace(*component.owner, component.data);
    }

    void remove_component(entity e, id::id_type component)
    {
        assert(has_component(e, component));

        if (has_component(e, component))
        {
            m_component_registry[component].remove(e);
        }
    }

    void* get_component(entity e, id::id_type component)
    {
        assert(has_component(e, component));

        if (has_component(e, component))
            return m_component_registry[component][e.get_id()];

        return nullptr;
    }
}

