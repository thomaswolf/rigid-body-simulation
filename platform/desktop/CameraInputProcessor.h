/* 
 * Simple WASD movement for a camera
 */
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "InputManager.h"
#include "Camera.h"

class CameraInputProcessor : public InputProcessor
{

private:
	Camera* camera;

public:

	CameraInputProcessor(Camera* camera)
	{
		this->camera = camera;
	}

	void OnKeyInput(const bool* keys, GLfloat dt) 
	{
		if (keys[GLFW_KEY_W]) camera->ProcessKeyboard(Camera_Movement::FORWARD, dt);
		if (keys[GLFW_KEY_S]) camera->ProcessKeyboard(Camera_Movement::BACKWARD, dt);
		if (keys[GLFW_KEY_A]) camera->ProcessKeyboard(Camera_Movement::LEFT, dt);
		if (keys[GLFW_KEY_D]) camera->ProcessKeyboard(Camera_Movement::RIGHT, dt);
	}

	void OnKeyDown(int key)
	{
		if (key == GLFW_KEY_E)
		{
			camera->SetSpeed(SPEED * 4);
		}
		else
		{
			camera->SetSpeed(SPEED);
		}
	}

	void SetCamera(Camera* camera)
	{
		this->camera = camera;
	}

	void OnMouseMoved(GLfloat xpos, GLfloat ypos, GLfloat xoffset, GLfloat yoffset)
	{
		camera->ProcessMouseMovement(xoffset, yoffset);
	}

	void OnScrollInput(GLfloat xoffset, GLfloat yoffset)
	{
		camera->ProcessMouseScroll(yoffset);
	}
};
