/*
 * Represents a generic object living in a scene that can be updated and might be drawn and can have children.
 * Children will move in the local coordinate system given by the parent.
 * (or everything you can click on in the scene if there would be an editor)
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <vector>

class Entity
{

protected:
	vec3 position;
	vec3 scale;
	quat rotation;

private:
	bool dirty = true; // true if model matrix needs to be updated
	mat4 M; // cached transform matrix
	Entity* parent = NULL;
	std::vector<Entity*> children;

public:
	Entity(vec3 pos, vec3 scale = vec3(1,1,1), quat rot = quat(vec3(0,0,0)))
	{
		this->position = pos;
		this->scale = scale;
		this->rotation = rot;
	}
	virtual ~Entity() 
	{
		for (Entity* child : children) delete child;	
		children.clear();
	}

	virtual void SetRotation(vec3 eulerAngles){	SetRotation(quat(eulerAngles)); }
	virtual void SetRotation(quat angles){	rotation = angles; dirty = true; }
	virtual void SetScale(vec3 scale){	this->scale = scale; dirty = true; }
	virtual void SetPosition(vec3 pos){	this->position = pos; dirty = true; }

	virtual vec3 GetPosition(){ return this->position; }
	virtual vec3 GetScale(){	return this->scale; }
	virtual quat GetRotation(){	return this->rotation; }
	
	// can be called manually to propagate information
	virtual void OnParentChanged()
	{
		for (Entity* child : children)
		{
			child->OnParentChanged();
		}	
	}; 

	virtual void SetGlobalPosition(vec3 pos)
	{
		if (parent == NULL) this->position = pos;
		else 
		{
			mat4 T = inverse(parent->GetTransformMatrix());
			vec4 pos4 = vec4(pos, 1);
			pos4 = T*pos4;
			this->position.x = pos4.x;
			this->position.y = pos4.y;
			this->position.z = pos4.z;
		}
		dirty = true;
	}

	virtual vec3 GetGlobalPosition()
	{
		if (parent == NULL) return this->position;
		else 
		{
			mat4 T = parent->GetTransformMatrix();
			vec4 pos = vec4(this->position, 1);
			pos = T*pos;
			return vec3(pos.x, pos.y, pos.z);
		}
	}

	virtual void SetGlobalRotation(quat rot)
	{
		if (parent == NULL) this->rotation = rot;
		else 
		{
			quat invRot = inverse(parent->rotation);
			this->rotation = invRot * rot;
		}
		dirty = true;
	}

	virtual quat GetGlobalRotation()
	{
		if (parent == NULL) return this->rotation;
		else 
		{
			return parent->rotation * this->rotation;
		}
	}

	virtual void SetGlobalScale(vec3 scale)
	{
		if (parent == NULL) this->scale = scale;
		else 
		{
			this->scale.x = 1./parent->scale.x * scale.x;
			this->scale.y = 1./parent->scale.y * scale.y;
			this->scale.z = 1./parent->scale.z * scale.z;
		}
		dirty = true;
	}

	virtual vec3 GetGlobalScale()
	{
		if (parent == NULL) return this->scale;
		else 
		{
			vec3 s = parent->scale;
			s.x = s.x * scale.x;
			s.y = s.y * scale.y;
			s.z = s.z * scale.z;
			return s;
		}
	}

	std::vector<Entity*> GetChildren(){	return this->children; }

	virtual void Update(double dt)
	{
   		for (Entity* child : children)
		{
			child->Update(dt);	
		}	
	}

	virtual void AddChild(Entity* e)
	{
		this->children.push_back(e);
		e->parent = this;
		e->OnParentChanged();
	}

	std::vector<Entity*> RemoveChildrenButKeepPosition()
	{
		std::vector<Entity*> cpy;

		for (Entity* e : children)
		{
			e->SetPosition(e->GetGlobalPosition());
			e->SetRotation(e->GetGlobalRotation());
			e->SetScale(e->GetGlobalScale());
			e->parent = NULL;
			e->OnParentChanged();
			cpy.push_back(e);
		}
		children.clear();

		return cpy;
	}

	virtual void Draw(Shader& shader)
	{
		for (Entity* child : children)
		{
			child->Draw(shader);	
		}
	}

	void SetDirty() { this->dirty = true; }

	mat4 GetTransformMatrix()
	{
		if (dirty)
		{
			M = mat4(1.0f); 

			// apply translation
			M = glm::translate(M, this->position);

			// apply rotation
			rotation = normalize(rotation);
			mat4 R = glm::mat4_cast(rotation);
			M = M*R; 

			// apply scale
			M = glm::scale(M, this->scale);

			dirty = false;
		}

		if (parent == NULL) return M;
		else return parent->GetTransformMatrix() * M;
	}

};
