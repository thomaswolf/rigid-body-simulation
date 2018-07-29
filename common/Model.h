/*
 * A model is an entity with a mesh supports instancing.
 * Instancing allows to render many objects with only one draw call.
 */	

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <vector>
#include "Entity.h"
#include "Mesh.h"

class Model : public Entity
{
private:
	

	// instancing
	bool slave = false; // true iff object is instance of other object
	std::vector<Model*> instances;

	// rendering
	bool visible = true;

	// model matrices
	mat4* modelMatrices = NULL; // space for model matrices for all instances

protected:

	Mesh *mesh;

public:

	Model(Mesh* mesh, vec3 pos) : Model(mesh, pos, false)
	{
	}

	Model(Mesh* mesh, vec3 pos, bool slave) : Entity(pos), mesh(mesh)
	{
		this->slave = slave;
	}

	virtual ~Model()
	{
		if (!slave && mesh != NULL) delete mesh;		
		if (modelMatrices != NULL) delete[] modelMatrices;
	}

	void SetVisible(bool v)
	{
		this->visible = v;
	}


	// creates an instance of the current model with the same mesh  : Model(
	// (can be rendered very quickly together with the current model)
	Model* CreateInstance(vec3 pos)
	{
		assert(slave == false);

		Model* instance = copyAsSlave(pos);
		instance->scale = scale;
		instance->rotation = rotation;
		instances.push_back(instance);

		// reset model matrix memory
		if (modelMatrices != NULL) delete[] modelMatrices;
		modelMatrices = NULL; 

		return instance;
	}

	
	virtual void Draw(Shader& shader)
	{
		if (!visible) return;

		Entity::Draw(shader);

		if (slave) return; // slave is rendered by master

		size_t n = instances.size() + 1; // plus one for the current instance

		if (n > 1)
		{
			// reserve memory for model matrices of all instances 
			if (modelMatrices == NULL) modelMatrices = new mat4[n];

			// we are master and render all instances of the current object
			for (int i = 0; i<n-1; ++i)
			{
				modelMatrices[i] = instances[i]->GetTransformMatrix();
			}
			modelMatrices[n-1] = GetTransformMatrix();
			
			glBindBuffer(GL_ARRAY_BUFFER, mesh->MBO);
			glBufferData(GL_ARRAY_BUFFER, n * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
		else
		{
			// we have no instances, render just this object
			mat4 M = GetTransformMatrix();
			glBindBuffer(GL_ARRAY_BUFFER, mesh->MBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &M[0], GL_STATIC_DRAW);
		}

		mesh->Draw(shader, n);
	}


protected:
	virtual Model* copyAsSlave(vec3 pos)
	{
		return new Model(mesh, pos, true);
	}
};
