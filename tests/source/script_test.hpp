#pragma once

#include "include/fabric.hpp"

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

	Transform operator * (float ts)
	{
		Transform result;
		for (int i = 0; i < 3; i++)
		{
			result.position[i] = position[i] * ts;
		}

		return result;
	}
};

class script_test
{
public:
	bool initialize();

	void update();

	float speed;

private:
	int test;
};