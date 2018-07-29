/*
 * Most basic scenegraph - just a vector of all entities
 */

#pragma once

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "PhysicManager.h"
#include "RigidBody.h"
#include "RigidBodyModel.h"

#include "vector"


class Scene
{

private:

	std::vector<Entity*> entities;
	std::list<Entity*> entitiesToAdd;

	// special entities that can exist only once
	Camera* camera;
	Light* light;
	PhysicManager* physicManager;

public:
	Scene()
	{
		// add physic manager
		physicManager = new PhysicManager();
	}

	~Scene()
	{
		delete physicManager;

		for (Entity* e : entities)
		{
			delete e;
		}

		entities.clear();

	}

	PhysicManager* GetPhysicManager()
	{
		return physicManager;
	}

	void StartSimulation()
	{
		this->physicManager->Start();
	}

	void StopSimulation()
	{
		this->physicManager->Stop();
	}

	void Clear()
	{
		physicManager->Clear();

		for (Entity* e : entities)
		{
			delete e;
		}
		for (Entity* e : entitiesToAdd)
		{
			delete e;
		}
		entities.clear();
		entitiesToAdd.clear();
	}

	void Draw(Shader& shader)
	{
		for (Entity* e : entities)
		{
 			e->Draw(shader);
		}
	}

	void AddEntities()
	{
		while (!entitiesToAdd.empty())
		{
			Entity* e = entitiesToAdd.front();
			entitiesToAdd.pop_front();

			entities.push_back(e);
			addEntityToPhysicManager(e);
		}
	}

	void Stabalize(GLfloat T)
	{
		AddEntities();
		physicManager->Stabilize(T);
	}

	void Update(double dt)
	{
		AddEntities();

		physicManager->Update(dt);

		int n = entities.size();

		//#pragma omp parallel for
		for (int i=0; i<n; ++i)
		{
			Entity* e = entities[i];
			e->Update(dt);
		}
	}

	void AddEntity(Entity* entity)
	{
		entitiesToAdd.push_back(entity);
	}

	void AddEntityAndFlattenHierachy(Entity* entity)
	{
		std::vector<Entity*> children = entity->RemoveChildrenButKeepPosition();
		for (Entity* e : children)
		{
			AddEntityAndFlattenHierachy(e);
		}

		entitiesToAdd.push_back(entity);
	}

	void SetCamera(Camera* camera)
	{
		this->camera = camera;
		this->entities.push_back(camera);
	}

	Camera* GetCamera()
	{
		return camera;	
	}

	void SetLight(Light* light)
	{
		this->light = light;
		this->entities.push_back(light);
	}

	Light* GetLight()
	{
		return light;	
	}

private:

	void addEntityToPhysicManager(Entity* entity)
	{
		if (RigidBodyModel* model = dynamic_cast<RigidBodyModel*>(entity))
		{
			physicManager->AddBody(model->GetRigidBody());
		}

		for (Entity* child : entity->GetChildren())
		{
			addEntityToPhysicManager(child);
		}
	}
};
