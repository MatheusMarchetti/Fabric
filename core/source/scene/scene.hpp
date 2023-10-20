#pragma once

#include "common.hpp"
#include "../utilities/type_list.hpp"

namespace fabric::ecs
{
	class entity;
	struct None {};

	extern u32 _componentCounter;

	struct component
	{
		entity* owner;
		id::id_type id;
		void* data = nullptr;
		u64 size = 0;
	};

	template<typename Component>
	const id::id_type get_component_id()
	{
		static id::id_type id = id::id_type(_componentCounter++);
		return id;
	}

	bool has_component(entity e, id::id_type component);
	void add_component(component& component);
	void remove_component(entity e, id::id_type component);
	void* get_component(entity e, id::id_type component);

	class entity
	{
	public:
		explicit entity() : m_id{ id::invalid_id } {}
		explicit entity(id::id_type id) : m_id(id) {}

		id::id_type get_id() { return m_id; }

		template<typename Component>
		constexpr bool has_component()
		{
			return ecs::has_component(*this, ecs::get_component_id<Component>());
		}

		template<typename Component>
		constexpr void add_component()
		{
			ecs::component component
			{
				.owner = this,
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
				.owner = this,
				.id = ecs::get_component_id<Component>(),
				.data = &component,
				.size = sizeof(Component)
			};

			ecs::add_component(comp);
		}

		template<typename Component>
		constexpr Component& get_component()
		{
			Component* component = (Component*)ecs::get_component(*this, ecs::get_component_id<Component>());

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
			ecs::remove_component(*this, ecs::get_component_id<Component>());
		}

	private:
		id::id_type m_id;
	};

	entity create_entity();
	void remove_entity(entity e);
	bool is_alive(entity e);
	std::span<entity> get_entities_with(id::id_type id);

	template<typename Component>
	std::span<entity> get_entities_with()
	{
		return get_entities_with(get_component_id<Component>());
	}

	template<typename Component>
	using Owner = typename utl::type_list<Component>;

	template<typename Head, typename... Tail>
	using Dependencies = typename utl::type_list<Head, Tail...>;

	template<typename Component, typename Head, typename... Tail>
	struct system
	{
		Owner<Component> owner;
		Dependencies<Head, Tail...> dependencies;
		void (*function)();
	};

	/// Convenience macro for registering systems.
	/// @param OWNER: Component that owns the system
	/// @param FN: Callback to system behavior function
	/// @param ...: Dependencies that must be resolved before this system can run
#define REGISTER_SYSTEM(OWNER, FN, ...)							\
		ecs::Owner<OWNER> OWNER##_ownership;					\
		ecs::Dependencies<__VA_ARGS__> OWNER##_dependencies;	\
		ecs::system<OWNER, __VA_ARGS__> OWNER##_system		\
		{														\
			.owner = OWNER##_ownership,							\
			.dependencies = OWNER##_dependencies,				\
			.function = &FN										\
		};														\
		ecs::register_system(OWNER##_system);
	
	id::id_type register_system(id::id_type owner, void(*function)());
	void add_dependency(id::id_type system_id, id::id_type dependency);

	// NOTE: The template requirement ensures that the user doesn't add the owner as a dependency
	template<typename Component, typename Head, typename... Tail>
		requires is_not_same<Component, Head>
	void register_system(system<Component, Head, Tail...>& sys)
	{
		id::id_type id = ecs::register_system(ecs::get_component_id<utl::front_t<Owner<Component>>>(), sys.function);
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
		id::id_type id = ecs::register_system(ecs::get_component_id<utl::front_t<Owner<Component>>>(), sys.function);
		ecs::add_dependency(id, ecs::get_component_id<utl::front_t<Dependencies<Head>>>());
	}
}

namespace std
{
	template<>
	struct hash<fabric::id::id_type>
	{
		size_t operator()(fabric::id::id_type const& id) const noexcept { return u64(id); }
	};
}
