#define TEST_ENTITY_COMPONENT 1
#define TEST_SERIALIZATION 0

#if TEST_ENTITY_COMPONENT
#include "entity_component.hpp"
#elif TEST_SERIALIZATION
#include "serialization_test.hpp"
#else
#error One of the tests need to be enabled
#endif

int main()
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	engine_test test{};

	if (test.initialize())
	{
		test.run();
	}

	test.shutdown();

	return 0;
}