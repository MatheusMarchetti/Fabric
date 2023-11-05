#pragma once

#include "common.hpp"

namespace detail
{
	template<typename Component>
	const fabric::id::id_type get_component_id()
	{
		static fabric::id::id_type id = fabric::id::id_type(typeid(Component).hash_code());
		return id;
	}
}

namespace fabric::ecs
{
	class entity;

	struct component
	{
		entity* owner;
		id::id_type id;
		void* data = nullptr;
		u32 size = 0;
	};

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
			return ecs::has_component(*this, detail::get_component_id<Component>());
		}

		template<typename Component>
		constexpr void add_component()
		{
			component component
			{
				.owner = this,
				.id = detail::get_component_id<Component>(),
				.size = sizeof(Component)
			};

			ecs::add_component(component);
		}

		template<typename Component>
		constexpr void add_component(Component& c)
		{
			component component
			{
				.owner = this,
				.id = detail::get_component_id<Component>(),
				.data = &c,
				.size = sizeof(Component)
			};

			ecs::add_component(component);
		}

		template<typename Component>
		constexpr utl::ref<Component> get_component()
		{
			return (Component*)ecs::get_component(*this, detail::get_component_id<Component>());
		}

		template<typename Component>
		constexpr void remove_component()
		{
			ecs::remove_component(*this, detail::get_component_id<Component>());
		}

	private:
		id::id_type m_id;
	};

	entity create_entity();
	void remove_entity(entity e);
	bool is_alive(entity e);
	utl::span<entity> get_entities_with(id::id_type id);

	template<typename Component>
	utl::span<entity> get_entities_with()
	{
		return get_entities_with(detail::get_component_id<Component>());
	}
	
	id::id_type add_system(id::id_type owner, void(*function)());
	void add_dependency(id::id_type system_id, id::id_type dependency);

	// NOTE: The template requirement ensures that the user doesn't add the owner as a dependency
	template<typename Owner, typename Head, typename... Tail>
		requires is_not_same<Owner, Head>
	void register_system(void(*function)())
	{
		id::id_type id = ecs::add_system(detail::get_component_id<Owner>(), function);
		ecs::add_dependency(id, detail::get_component_id<Head>());

		if constexpr (sizeof...(Tail) != 0)
			register_system<Owner, Tail...>(function);
	}

	template<typename Owner>
	void register_system(void(*function)())
	{
		id::id_type id = ecs::add_system(detail::get_component_id<Owner>(), function);
	}
}

namespace std
{
	template<>
	struct hash<fabric::id::id_type>
	{
		size_t operator()(fabric::id::id_type const& id) const noexcept { return size_t(id); }
	};
}
