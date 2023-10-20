#include "registry.hpp"
#include "scene.hpp"

namespace fabric::ecs
{
    u32 _componentCounter = 0;
    
    namespace
    {
        registry m_registry;

        // System storage - temporary
        std::unordered_map<id::id_type, void(*)()> m_system_registry;
    }

    entity create_entity()
    {
        return entity(m_registry.create_entity());
    }

    void remove_entity(entity e)
    {
        assert(is_alive(e));
        
        if (is_alive(e))
        {
            m_registry.remove_entity(e.get_id());
        }
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
        size_t size = m_registry.get_component_count(component_id);
        entity* entities = m_registry.get_component_storage(component_id).dense();

        return std::span<entity>(entities, size);
    }

    bool has_component(entity e, id::id_type component)
    {
        if (m_registry.component_exists(component) && m_registry.get_component_storage(component).has(e))
            return true;

        return false;
    }

    void add_component(component& component)
    {
        if (!m_registry.component_exists(component.id))
            m_registry.register_component(component.id, component.size);

        m_registry.get_component_storage(component.id).emplace(*component.owner, component.data);
    }

    void remove_component(entity e, id::id_type component)
    {
        assert(has_component(e, component));

        if (has_component(e, component))
        {
            m_registry.get_component_storage(component).remove(e);
        }
    }

    void* get_component(entity e, id::id_type component)
    {
        assert(has_component(e, component));

        if (has_component(e, component))
            return m_registry.get_component_storage(component)[e.get_id()];

        return nullptr;
    }
}

