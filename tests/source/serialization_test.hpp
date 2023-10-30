#pragma once

#include "test.hpp"

#include "include/fabric.hpp"

#include <iostream>

using namespace fabric;

struct Transform
{
	float position[3];
	float rotation;

	Transform()
	{
		rotation = 5.0f;
	}
};

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
	for (u32 i = 0; i < 25; i++)
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

		if(i % 2)
			e.add_component<Audio>(a);

		if(i % 5)
			e.add_component<Geometry>(g);
	}

	ecs::save_scene();
}

class engine_test : public test
{
public:
	virtual bool initialize() override
	{
		//serialize();

		return ecs::load_scene();
	}

	virtual void run() override
	{
		auto entities = ecs::get_entities_with<Transform>();

		for (auto& entity : entities)
		{
			Transform& t = entity.get_component<Transform>();
			std::cout << t.position[0] << ", " << t.position[1] << ", " << t.position[2] << ", " << t.rotation << std::endl;
		}

		entities = ecs::get_entities_with<Audio>();
		entities = ecs::get_entities_with<Geometry>();

		std::cin.get();
	}

	virtual void shutdown() override
	{

	}
};