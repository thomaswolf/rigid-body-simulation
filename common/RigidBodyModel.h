#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cassert>
using namespace glm;

#include <list>

#include "Model.h"
#include "Shape.h"
#include "RigidBody.h"


/*
 * Links a rigidBody to a model that can be added to the scene and thus be rendered. 
 * RigidBody calculates in world coordinates only. Thus we need to transform the coordinates accordingly.
 */
class RigidBodyModel : public Model
{

private:
	RigidBody* body;

public:
	RigidBodyModel(Mesh* mesh, dvec3 pos) : RigidBodyModel(mesh, pos, false)
	{
	}

	RigidBodyModel(Mesh* mesh, dvec3 pos, bool slave) : Model(mesh, pos, slave)
	{
		body = new RigidBody(GetGlobalPosition(), new Shape(mesh));
		body->SetScale(GetGlobalScale());
		body->SetRotation(GetGlobalRotation());
	}

	virtual ~RigidBodyModel()
	{
		delete body;
	}

	RigidBody* GetRigidBody() { return body; }

	void SetStatic()
	{
		this->body->SetStatic();
	}

	virtual void Draw(Shader& shader) 
	{
		Model::Draw(shader);
	}

	virtual void OnParentChanged()
	{
		Model::OnParentChanged();
		this->body->SetPosition(GetGlobalPosition());
		this->body->SetRotation(GetGlobalRotation());
		this->body->SetScale(GetGlobalScale());
	}; 

	virtual void SetRotation(vec3 eulerAngles) 
	{
		this->SetRotation(quat(eulerAngles));
	}

	virtual void SetRotation(quat angles) 
	{
		Model::SetRotation(angles);
		this->body->SetRotation(GetGlobalRotation());
	}

	virtual void SetPosition(vec3 position) 
	{
		Model::SetPosition(position);
		this->body->SetPosition(GetGlobalPosition());
	}

	virtual void SetScale(vec3 scale)
	{
		Model::SetScale(scale);
		this->body->SetScale(GetGlobalScale());
	}

	void Update(double dt)
	{
		Entity::SetGlobalPosition(body->GetPosition());
	   	Entity::SetGlobalRotation(body->GetRotation());
	}


protected:

	// creates an instance of the current model with the same mesh  
	virtual Model* copyAsSlave(vec3 pos)
	{
		return new RigidBodyModel(mesh, pos, true);
	}

};

