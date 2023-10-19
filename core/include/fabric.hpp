#pragma once

#include "engine.hpp"

namespace fabric
{
	namespace entity
	{
		class entity
		{
		public:
			explicit entity() : m_id{ id::invalid_id } {}
			explicit entity(ecs::entity_id id) : m_id(id) {}

			ecs::entity_id get_id() { return m_id; }

			template<typename Component>
			constexpr bool has_component()
			{
				return ecs::has_component(m_id, ecs::get_component_id<Component>());
			}

			template<typename Component>
			constexpr void add_component()
			{
				ecs::component component
				{
					.owner = m_id,
					.id = ecs::get_component_id<Component>(),
					.size = sizeof(Component)
				};

				ecs::add_component(component);
			}

			template<typename Component>
			constexpr void add_component(Component& component)
			{
				ecs::component comp
				{
					.owner = m_id,
					.id = ecs::get_component_id<Component>(),
					.data = &component,
					.size = sizeof(Component)
				};

				ecs::add_component(comp);
			}

			template<typename Component>
			constexpr const Component& get_component()
			{
				Component* component = (Component*) ecs::get_component(m_id, ecs::get_component_id<Component>());

				if (!component)
				{
					Component comp = Component();
					component = &comp;
				}

				return *component;
			}

			template<typename Component>
			constexpr void remove_component()
			{
				ecs::remove_component(m_id, ecs::get_component_id<Component>());
			}

		private:
			ecs::entity_id m_id;
		};
	}

	namespace scene
	{
		entity::entity create_entity()
		{
			return entity::entity(ecs::create_entity());
		}

		void remove_entity(entity::entity e)
		{
			ecs::remove_entity(e.get_id());
		}

		// NOTE: The template requirement ensures that the user doesn't add the owner as a dependency
		template<typename Component, typename Head, typename... Tail> 
		requires is_not_same<Component, Head>
		void register_system(system<Component, Head, Tail...>& sys)
		{
			ecs::system_id id = ecs::register_system(ecs::get_component_id<utl::front_t<Owner<Component>>>(), sys.function);
			ecs::add_dependency(id, ecs::get_component_id<utl::front_t<Dependencies<Head>>>());
			
				Owner<Component> component_ownership;
				Dependencies<Tail...> component_dependencies;
				system<Component, Tail...> component_system
				{
					.owner = component_ownership,
					.dependencies = component_dependencies,
					.function = sys.function
				};

				register_system(component_system);
		}

		template<typename Component, typename Head> 
		requires is_not_same<Component, Head>
		void register_system(system<Component, Head>& sys)
		{
			ecs::system_id id = ecs::register_system(ecs::get_component_id<utl::front_t<Owner<Component>>>(), sys.function);
			ecs::add_dependency(id, ecs::get_component_id<utl::front_t<Dependencies<Head>>>());
		}

		void run_systems()
		{
			ecs::run_systems();
		}
	}
}
