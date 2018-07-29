#pragma once

#include "timer.h"
#include "Scene.h"


class Benchmark
{
private:
	Scene* scene;

public:

	Benchmark(Scene* scene)
	{
		this->scene = scene;
	}


	void Run()
	{
		scene->clear();

	}


}
