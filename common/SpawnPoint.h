#pragma once

#include "functional"
#include "Entity.h"
#include "Scene.h"

class SpawnPoint : public Entity
{

private:
   	float interval;
   	int n;
   	int i;
	double time = 0;
	std::function<Model*(int)> spawnFunc;
	Scene* scene;

public:

	SpawnPoint(Scene *s, vec3 pos, float interval, int n, std::function<Model*(int)> spawnFunc) : Entity(pos)
	{
		assert(s != NULL);

		this->interval = interval;
		this->scene = s;
		this->n = n;
		this->spawnFunc = spawnFunc;
		this->i = 0;
		this->time = 0;
	}
	

	virtual void Update(double dt)
	{
		if (i >= n) return;

		time -= dt;

		if (time  < 0)
		{
			Model* m = spawnFunc(i);
			m->SetPosition(this->position);
			scene->AddEntity(m);

			time = interval;
			i++;
		}
	}

};
