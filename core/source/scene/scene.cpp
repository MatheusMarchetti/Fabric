#include "scene.hpp"
#include "registry.hpp"

#include "source/utilities/graph.hpp"

namespace
{
    fabric::ecs::registry m_registry;
    fabric::utl::graph m_execution_graph;

    // System storage - temporary. Need to move inside the registry
    fabric::utl::unordered_map<fabric::id::id_type, void(*)()> m_system_registry;
}

namespace fabric::ecs
{
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

    id::id_type add_system(id::id_type owner, void(*function)())
    {
        m_system_registry[owner] = function;
        m_execution_graph.add_node(owner);
        return owner;
    }

    void add_dependency(id::id_type system_id, id::id_type dependency)
    {
        m_execution_graph.add_dependency(system_id, dependency);
    }

    utl::span<entity> get_entities_with(id::id_type component_id)
    {
        size_t size = m_registry.get_component_count(component_id);
        entity* entities = m_registry.get_component_storage(component_id).dense();

        return utl::span<entity>(entities, size);
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

namespace fabric::scene
{
    void initialize()
    {
        // register built-in systems
        // initialize user scripts

        m_execution_graph.build();
    }

    void update()
    {
        auto execution_queues = m_execution_graph.get_execution_order();

        for (auto& queue : execution_queues)
        {
            // TODO: Dispatch each queue as a sequence of jobs in the job system
            for (auto& entry : queue)
                m_system_registry[entry]();
        }
    }

    bool save()
    {
        // TODO: This should be handled by a VFS and accessed via a scene asset handle
        FILE* bin;
        fopen_s(&bin, "entities.bin", "wb");

        if (bin)
        {
            m_registry.serialize(bin);
            fclose(bin);

            return true;
        }

        return false;
    }

    bool load()
    {
        // TODO: This should be handled by a VFS and accessed via a scene asset handle
        FILE* bin;
        fopen_s(&bin, "entities.bin", "rb");

        if (bin)
        {
            bool result = m_registry.deserialize(bin);

            fclose(bin);

            return result;
        }

        return false;
    }

    void unload()
    {
        m_registry.clear();
        m_execution_graph.clear();
    }
}

