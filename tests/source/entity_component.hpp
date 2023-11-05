#pragma once

#include "test.hpp"

#include "include/fabric.hpp"

#include <iostream>
#include <ctime>

using namespace fabric;

struct Random
{
	u32 random;
};

struct Transform
{
	float position[3];

	void operator *=(float ts)
	{
		for (int i = 0; i < 3; i++)
		{
			position[i] *= ts;
		}
	}
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
		utl::ref<Transform> t = entity.get_component<Transform>();

		std::cout << "(Entity id: " << entity.get_id() << ") Transform before: [" << t->position[0] << ", " << t->position[1] << ", " << t->position[0] << "]" << std::endl;

		if (entity.has_component<Random>())
		{
			utl::ref<Random> r = entity.get_component<Random>();
			t *= (float)r->random;
		}

		t *= 2.0f;

		std::cout << "(Entity id: " << entity.get_id() << ") Transform after: [" << t->position[0] << ", " << t->position[1] << ", " << t->position[0] << "]" << std::endl;
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
		entity_test();
		component_test();

		scene::update();
		std::cin.get();
	}

	virtual void shutdown() override
	{  
		scene::save();
	}

private:
	void entity_test()
	{
		ecs::entity e1 = ecs::create_entity();
		ecs::entity e2 = ecs::create_entity();
		ecs::entity e3 = ecs::create_entity();
		ecs::entity e4 = ecs::create_entity();

		std::cout << "Entity id: " << e1.get_id() << std::endl;
		std::cout << "Entity id: " << e2.get_id() << std::endl;
		std::cout << "Entity id: " << e3.get_id() << std::endl;
		std::cout << "Entity id: " << e4.get_id() << std::endl;

		ecs::remove_entity(e1);
		ecs::remove_entity(e2);
		ecs::remove_entity(e3);
		ecs::remove_entity(e4);

		ecs::entity e5 = ecs::create_entity();
		ecs::entity e6 = ecs::create_entity();
		ecs::entity e7 = ecs::create_entity();
		ecs::entity e8 = ecs::create_entity();

		std::cout << "Entity id: " << e5.get_id() << std::endl;
		std::cout << "Entity id: " << e6.get_id() << std::endl;
		std::cout << "Entity id: " << e7.get_id() << std::endl;
		std::cout << "Entity id: " << e8.get_id() << std::endl;

		ecs::remove_entity(e5);
		ecs::remove_entity(e6);
		ecs::remove_entity(e7);
		ecs::remove_entity(e8);

		ecs::entity e9 = ecs::create_entity();
		ecs::entity e10 = ecs::create_entity();
		ecs::entity e11 = ecs::create_entity();
		ecs::entity e12 = ecs::create_entity();

		std::cout << "Entity id: " << e9.get_id() << std::endl;
		std::cout << "Entity id: " << e10.get_id() << std::endl;
		std::cout << "Entity id: " << e11.get_id() << std::endl;
		std::cout << "Entity id: " << e12.get_id() << std::endl;

		ecs::remove_entity(e9);
		ecs::remove_entity(e10);
		ecs::remove_entity(e11);
		ecs::remove_entity(e12);
	}

	void component_test()
	{
		Random r;
		r.random = rand() % 20;

		Transform transform;
		transform.position[0] = 1.0f;
		transform.position[1] = 2.0f;
		transform.position[2] = 3.0f;

		ecs::entity e1 = ecs::create_entity();

		e1.add_component<Tag>();

		e1.add_component<Random>();

		utl::ref<Transform> t1 = e1.get_component<Transform>();

		if (t1)
		{
			t1->position[0] = 25.f;
			std::cout << t1->position[0] << std::endl;
		}

		auto r1 = e1.get_component<Random>();

		e1.add_component<Transform>(transform);

		if (r1)
		{
			std::cout << "Random before substitution: " << r1->random << std::endl;

			e1.add_component<Random>(r);
			std::cout << "Random after substitution: " << r1->random << std::endl;

			r.random *= 5;
			r1 = r;
			std::cout << "Random after overwrite: " << r1->random << std::endl;
		}

		transform *= 10.0f;

		if (e1.has_component<Transform>())
		{
			e1.add_component<Transform>(transform);

			utl::ref<Transform> t = e1.get_component<Transform>();

			std::cout << "Position X: " << t->position[0] << std::endl;
			std::cout << "Position Y: " << t->position[1] << std::endl;
			std::cout << "Position Z: " << t->position[2] << std::endl;

		}

		e1.remove_component<Tag>();
		e1.remove_component<Random>();
		e1.remove_component<Transform>();

		ecs::remove_entity(e1);
	}

	void system_test()
	{
		Transform transform;
		transform.position[0] = 1.0f;
		transform.position[1] = 2.0f;
		transform.position[2] = 3.0f;

		for (int i = 0; i < 100; i++)
		{
			ecs::entity e = ecs::create_entity();
			Random r;
			r.random = rand() % 20;
			e.add_component<Random>(r);
			e.add_component<Transform>(transform);
		}

		ecs::register_system<Transform, Random, Tag>(update_transforms);
		ecs::register_system<Random>(system_func);
		ecs::register_system<Tag, Random>(system_func);
	}

private:
	u64 _count = 0;
};