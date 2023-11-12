#pragma once

#include "test.hpp"

#include "include/fabric.hpp"

#include "script_test.hpp"

#include <iostream>

using namespace fabric;

struct Audio
{
	float volume;
};

struct Geometry
{
	u32 mesh_id;
};

void serialize()
{
	u32 entity_count = 10;

	for (u32 i = 0; i < entity_count; i++)
	{
		Transform t;
		t.position[0] = 0.0f;
		t.position[1] = 1.0f;
		t.position[2] = 2.0f;

		Audio a;
		a.volume = 10.0f;

		Geometry g;
		g.mesh_id = 7;

		ecs::entity e = ecs::create_entity();
		e.add_component<Transform>(t);

		e.add_component<script_test>();
		auto s = e.get_component<script_test>();
		s->speed = 10.f * rand();

		if(i % 2)
			e.add_component<Audio>(a);

		if(i % 5)
			e.add_component<Geometry>(g);
	}

	scene::save();

	auto entities = ecs::get_entities_with<Transform>();

	for (auto& entity : entities)
	{
		ecs::remove_entity(entity);
	}
}

class engine_test : public test
{
public:
	virtual bool initialize() override
	{
		serialize();

		return scene::load();
	}

	virtual void run() override
	{
		auto entities = ecs::get_entities_with<Transform>();

		for (auto& entity : entities)
		{
			auto t = entity.get_component<Transform>();
			std::cout << t->position[0] << ", " << t->position[1] << ", " << t->position[2] << std::endl;

			auto s = entity.get_component<script_test>();

			std::cout << s->speed << std::endl;
		}

		entities = ecs::get_entities_with<Audio>();
		entities = ecs::get_entities_with<Geometry>();

		std::cin.get();
	}

	virtual void shutdown() override
	{

	}
};