#pragma once

#include "test.hpp"

#include "include/fabric.hpp"

#include <iostream>
#include <typeinfo>

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

void serialize()
{
	for (u32 i = 0; i < 25; i++)
	{
		Transform t;
		t.position[0] = 0.0f;
		t.position[1] = 1.0f;
		t.position[2] = 2.0f;

		ecs::entity e = ecs::create_entity();
		e.add_component<Transform>(t);
	}

	ecs::save_scene();
}

class engine_test : public test
{
public:
	virtual bool initialize() override
	{
		serialize();

		return ecs::load_scene();
	}

	virtual void run() override
	{
		std::cout << "Transform ID via typeid::hash_code(): " << typeid(Transform).hash_code() << std::endl;

		std::cin.get();
	}

	virtual void shutdown() override
	{

	}
};