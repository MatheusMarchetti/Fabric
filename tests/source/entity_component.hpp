#pragma once

#include "test.hpp"

#include "include/fabric.hpp"

#include "script_test.hpp"

#include <iostream>
#include <ctime>

using namespace fabric;

struct Random
{
	u32 random;
};

struct Tag 
{
	char* tag = nullptr;
};

void system_func()
{
	std::cout << "System working!" << std::endl;
}

void update_transforms()
{
	std::cout << std::endl << "Updating all transforms..." << std::endl << std::endl;

	auto entities = ecs::get_entities_with<Transform>();

	for (auto& entity : entities)
	{
		Transform transform;
		utl::ref<Transform> t = entity.get_component<Transform>();

		std::cout << "(Entity id: " << entity.get_id() << ") Transform before: [" << t->position[0] << ", " << t->position[1] << ", " << t->position[2] << "]" << std::endl;

		if (entity.has_component<Random>())
		{
			utl::ref<Random> r = entity.get_component<Random>();
			t *= (float)r->random;
		}

		t *= 2.0f;

		transform = t * 50.0f;

		std::cout << "(Entity id: " << entity.get_id() << ") Transform after: [" << t->position[0] << ", " << t->position[1] << ", " << t->position[2] << "]" << std::endl;
	}
}

class engine_test : public test
{
public:
	virtual bool initialize() override
	{
		srand((u32)time(nullptr));

		system_test();

		scene::initialize();

		return true;
	}

	virtual void run() override
	{ 
		ecs_test();

		scene::update();
		std::cin.get();
	}

	virtual void shutdown() override
	{  
		scene::save();
	}

private:
	void ecs_test()
	{
		utl::vector<ecs::entity> entities = ecs::get_entities_with<Transform>();

		for (auto& entity : entities)
		{
			entity.add_component<script_test>();
		}

		ecs::entity e1 = entities[0];

		std::cout << "Setting entity 0 script_test 'speed' to 5 \n";
		auto s1 = e1.get_component<script_test>();
		s1->speed = 5.f;
	}

	void system_test()
	{
		Transform transform;
		transform.position[0] = 1.0f;
		transform.position[1] = 2.0f;
		transform.position[2] = 3.0f;

		for (int i = 0; i < 2; i++)
		{
			ecs::entity e = ecs::create_entity();
			Random r;
			r.random = rand() % 20;
			e.add_component<Random>(r);
			e.add_component<Transform>(transform);
		}

		ecs::register_system<Transform, Random, Tag>(update_transforms);
		ecs::register_system<Random>(nullptr);
		ecs::register_system<Tag, Random>(system_func);
	}

private:
	u64 _count = 0;
};