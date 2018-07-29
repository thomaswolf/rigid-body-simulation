/*
 * Stores data for the directionl light stored in the scene and used by the renderManager
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "Entity.h"

class Light : public Entity
{

private:
	mat4 lightProjection;
	mat4 lightSpaceMatrix;
	vec3 target;
	vec3 color;

public:

	Light(vec3 pos, vec3 target, GLfloat size = 10, GLfloat near_plane = 2, GLfloat far_plane = 15.5f) : Entity(pos)
	{
		SetTarget(target);
		lightProjection = glm::ortho(-size, size, -size, size, near_plane, far_plane);
		color = vec3(0.8);
	}
	
	void SetTarget(vec3 target)
	{
		this->target = target;	
	}

	void SetColor(vec3 color)
	{
		this->color = color;
	}

	vec3 GetColor() { return color; }
	
	// transforms from world space to light space (the light is modeled just like a camera)
	mat4 GetLightSpaceMatrix()
	{
		mat4 lightView = glm::lookAt(this->position, this->target, vec3(0.0, 1.0, 0.0));
		return lightProjection * lightView;
	}
};


class MovingLight : public Light
{

public:
	MovingLight(vec3 pos, vec3 target) : Light(pos, target) {}

	// little bit movement to make it more interesting
	GLfloat time = 0;
	void Update(double dt)
	{
		time += 0.2*dt;
		this->position.x = 10*std::cos(time);
		this->position.z = 5*std::sin(time);
	}
};
