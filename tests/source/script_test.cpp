#include "script_test.hpp"
#include <string>
#include <iostream>

#include "EntitySpawner.hpp"

using namespace fabric;

REGISTER_SCRIPT(script_test)
bool script_test::initialize()
{
	std::cout << "Created script script_test! \n";

	ecs::register_system<script_test, Transform>(BIND_PROC(script_test::update));

	return true;
}

void script_test::update()
{
	utl::vector<ecs::entity> entities = ecs::get_entities_with<script_test>();

	for (auto& entity : entities)
	{
		if (auto t = entity.get_component<Transform>())
		{
			auto s = entity.get_component<script_test>();
			std::cout << "Entity that contains script_test is: " << entity.get_id() << ", its transform component is: "
				<< t->position[0] << ", " << t->position[1] << ", " << t->position[2] << " and script_test 'speed' is: " << s->speed << std::endl;
		}

		if(!entity.has_component<EntitySpawner>())
		{
			std::cout << "Adding EntitySpawner to entity " << entity.get_id() << std::endl;
			entity.add_component<EntitySpawner>();
		}
	}
}
