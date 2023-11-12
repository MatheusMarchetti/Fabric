#pragma once

#include "../source/memory/memory.hpp"
#include "../source/scene/scene.hpp"

// TODO: Move inside the engine. Only load/save should be exposed
// NOTE: Scene will probably be a standalone class

namespace fabric::scene
{
	void initialize();
	void update();
	bool save();
	bool load();
	void unload();

	u8 register_script(id::id_type id, void* (*creator)());

#ifdef USE_WITH_EDITOR
	u8 add_script_name(const char* name, id::id_type id);
#endif
}

#ifdef USE_WITH_EDITOR
#define REGISTER_SCRIPT(script)\
	namespace																											\
	{																													\
		static script instance;																							\
		static bool initialized = false;																				\
																														\
		void* instance_creator()																						\
		{																												\
			instance = script();																						\
																														\
			if (!initialized)																							\
			{																											\
				initialized = instance.initialize();																	\
			}																											\
																														\
			return &instance;																							\
		}																												\
																														\
		u8 script##_reg = fabric::scene::register_script(detail::get_component_id<script>(), &instance_creator);		\
																														\
		u8 script##_name = fabric::scene::add_script_name(#script, detail::get_component_id<script>());					\																												\
	}
#else
#define REGISTER_SCRIPT(script)\
	namespace																											\
	{																													\
		static script instance;																							\
		static bool initialized = false;																				\
																														\
		void* instance_creator()																						\
		{																												\
			instance = script();																						\
																														\
			if (!initialized)																							\
			{																											\
				initialized = instance.initialize();																	\
			}																											\
																														\
			return &instance;																							\
		}																												\
																														\
		u8 script##_reg = fabric::scene::register_script(detail::get_component_id<script>(), &instance_creator);		\
																														\
	}
#endif