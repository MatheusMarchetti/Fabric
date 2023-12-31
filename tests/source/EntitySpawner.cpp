#include "EntitySpawner.hpp"
#include <iostream>

#include "script_test.hpp"

struct Counter {};

using namespace fabric;

REGISTER_SCRIPT(EntitySpawner)

void update()
{
	for (auto entity : ecs::get_entities_with<EntitySpawner>())
	{
		std::cout << "Spawning entity!" << std::endl;

		ecs::entity e = ecs::create_entity();

		e.add_component<Counter>();

		EntitySpawner::EntityCount++;
	}

	std::cout << "Spawned count: " << EntitySpawner::EntityCount << std::endl;
}

bool EntitySpawner::initialize()
{
	ecs::register_system<EntitySpawner, script_test>(update);

	return true;
}
