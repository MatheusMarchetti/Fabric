#pragma once

#include "../source/memory/memory.hpp"
#include "../source/scene/scene.hpp"

// TODO: Move inside the engine. Only load/save should be exposed
namespace fabric::scene
{
	void initialize();
	void update();
	bool save();
	bool load();
	void unload();
}