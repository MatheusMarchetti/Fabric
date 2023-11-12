#include "script_test.hpp"
#include <string>

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

bool script_test::initialize()
{
	return true;
}
