#include "scene.hpp"
#include "registry.hpp"

#include "source/utilities/graph.hpp"

namespace
{
    fabric::ecs::registry m_registry;
    fabric::utl::graph m_execution_graph;

    using script_factory = void* (*)();
    using script_registry = fabric::utl::unordered_map<fabric::id::id_type, script_factory>;
    
    static void* m_script_instance;

    script_registry& get_registry()
    {
        static script_registry reg;
        return reg;
    }
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
        m_registry.register_system(owner, function);
        m_execution_graph.add_node(owner);

        return owner;
    }

    void add_dependency(id::id_type system_id, id::id_type dependency)
    {
        m_execution_graph.add_dependency(system_id, dependency);
    }

    utl::vector<entity> get_entities_with(id::id_type component_id)
    {
        size_t size = m_registry.get_component_count(component_id);
        entity* entities = m_registry.get_component_storage(component_id).dense();

        utl::vector<entity> e(size);
        e.assign(entities, entities + size);
        return e;
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

        m_registry.assign_component_to_entity(component.owner->get_id(), component.id);
        m_registry.get_component_storage(component.id).emplace(*component.owner, component.data);
    }

    void remove_component(entity e, id::id_type component)
    {
        assert(has_component(e, component));

        if (has_component(e, component))
        {
            m_registry.remove_component_from_entity(e.get_id(), component);
            m_registry.get_component_storage(component).remove(e);
        }
    }

    void* get_component(entity e, id::id_type component)
    {
        return has_component(e, component) ? m_registry.get_component_storage(component)[e.get_id()] : nullptr;
    }
}

namespace detail
{
    void* get_script_instance(fabric::id::id_type id)
    {
        m_script_instance = get_registry()[id]();
        return m_script_instance;
    }
}

namespace fabric::scene
{
    void initialize()
    {
        // register built-in systems
        
    }

    void update()
    {
        m_execution_graph.build();
        auto execution_queues = m_execution_graph.get_execution_order();

        for (auto& queue : execution_queues)
        {
            // TODO: Dispatch each queue as a sequence of jobs in the job system
            for (auto& entry : queue)
            {
                ecs::registry::system_proc proc = m_registry.get_system_proc(entry);

                if (proc) proc();
            }
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

    u8 register_script(id::id_type id, script_factory creator)
    {
        bool result = get_registry().insert(script_registry::value_type{ id, creator }).second;
        
        assert(result);

        return result;
    }
}

