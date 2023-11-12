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
}