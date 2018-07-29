/* 
 * Simple WASD movement for a camera
 */
#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "InputManager.h"
#include "Camera.h"

class Joystick
{

public:

	void OnTouch(int iPointerID, float fPosX, float fPosY, int iAction)
	{

	}
};



class CameraInputProcessor : public InputProcessor
{

private:
	Camera* camera;
	Scene* scene;


    float lastX;
    float lastY;


	bool looking = false;
	vec2 lookCenter;
	vec2 look;
	int lookPointer;
	int noLookUpdateSince = 0;

    bool moving = false;
	vec2 moveCenter;
	vec2 move;
	int movePointer;
	int noMoveUpdateSince = 0;

	double shootRadius = 0.2;
	vec2 shootBtn;

	bool shoot = false;


public:


	CameraInputProcessor(Scene* scene, Camera* camera)
	{
		this->scene = scene;
		this->camera = camera;
		this->shootBtn = vec2(0.5, 0.5);
	}

	void Update(double dt)
	{
		if (moving)
		{
			camera->MoveForward(move.y, dt*10);
			camera->MoveRight(move.x, dt*10);
		}

		if (looking)
		{
			camera->ProcessMouseMovement(look.x*50, look.y*50);
		}

		if (shoot && scene->GetPhysicManager()->IsRunning())
		{
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(1,0,0), 12), camera->GetPosition());
			ball->SetScale(vec3(0.1));
			ball->GetRigidBody()->ApplyLinearMomentum(5.0f * camera->GetDirection());
			scene->AddEntity(ball);
			shoot = false;
		}

		noMoveUpdateSince++;
		noLookUpdateSince++;


		// timeouts
		if (noMoveUpdateSince > 10)
		{
			moving = false;
		}
		if (noLookUpdateSince > 10)
		{
			looking = false;
		}
	}

	void OnTouch(int pointer, float x, float y, int action)
	{
		vec2 pos(x,y);

        float dx = x - lastX;
        float dy = y - lastY;

	    //ALOGV( "Touch: %i, x: %f y:, %f action:, %i. (%f, %f)", pointer, x, y, action, dx, dy);

		if (action == 0 && x > shootBtn.x - shootRadius &&
			x < shootBtn.x + shootRadius &&
			y > shootBtn.y -  shootRadius &&
			y < shootBtn.y + shootRadius)
		{
			shoot = true;
		}

	    if (x > 0.5 )
	    {
			if (action == 0 || (action == 2 && looking == false)) // touch down
			{
				lookCenter = vec2(x,y);
				looking = true;
				lookPointer = pointer;
				look = vec2(0,0);
			}

	    }
	    else
	    {
			if (action == 0 || (action == 2 && moving == false)) // touch down
			{
				moveCenter = vec2(x,y);
				moving = true;
				movePointer = pointer;
				move = vec2(0,0);
				noMoveUpdateSince = 0;
			}
	    }



		if (pointer == lookPointer && looking)
		{
			if (action == 1 || action == 6 || action == 3) // touch up
			{
				looking = false;
			}
			else if (action == 2) // touch move
			{
				looking = true;
				look = pos -  lookCenter;
				noLookUpdateSince = 0;
			}
		}
		else noLookUpdateSince++;

		if (pointer == movePointer && moving)
		{
			if (action == 1 || action == 6 || action == 3) // touch up
			{
				moving = false;
			}
			else if (action == 2) // touch move
			{
				moving = true;
				move = pos - moveCenter;
				noMoveUpdateSince = 0;
			}
		}
		else noMoveUpdateSince++;


		//if (keys[GLFW_KEY_W]) camera->ProcessKeyboard(Camera_Movement::FORWARD, dt);
		//if (keys[GLFW_KEY_S]) camera->ProcessKeyboard(Camera_Movement::BACKWARD, dt);
		//if (keys[GLFW_KEY_A]) camera->ProcessKeyboard(Camera_Movement::LEFT, dt);
		//if (keys[GLFW_KEY_D]) camera->ProcessKeyboard(Camera_Movement::RIGHT, dt);

		lastX = x;
		lastY = y;
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
