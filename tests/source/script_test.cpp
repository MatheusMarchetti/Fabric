#include "script_test.hpp"
#include <string>
#include <iostream>

using namespace fabric;

// REGISTER_SCRIPT
namespace
{
	static script_test instance;
	static bool initialized = false;

	void* instance_creator()
	{
		instance = script_test();

		if(!initialized)
		{
			initialized = instance.initialize();
		}

		return &instance;
	}

	u8 script_test_reg = fabric::scene::register_script(detail::get_component_id<script_test>(), &instance_creator);
}

void update()
{
	utl::vector<ecs::entity> entities = ecs::get_entities_with<script_test>();

	for (auto& entity : entities)
	{
		if (auto t = entity.get_component<Transform>())
		{
			auto s = entity.get_component<script_test>();
			std::cout << "Entity that contains script is: " << entity.get_id() << ", its transform component is: "
				<< t->position[0] << ", " << t->position[1] << ", " << t->position[2] << " and script 'speed' is: " << s->speed << std::endl;
		}
	}
}

bool script_test::initialize()
{
	std::cout << "Created script! \n";

	ecs::register_system<script_test, Transform>(update);

	return true;
}
