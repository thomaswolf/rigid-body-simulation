/*
 * Input processor to play around with settings
 */
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "InputManager.h"
#include "DesktopWindowManager.h"


class DebugInputProcessor : public InputProcessor
{
private:

	Scene* scene;
	bool simulationRunning = true;
	bool debugRendering = true;
	bool fullscreen = false;
	DesktopWindowManager* windowManager;

public:
	
	DebugInputProcessor(DesktopWindowManager* windowManager, Scene* scene)
	{
		this->scene = scene;
		this->windowManager = windowManager;
		if (debugRendering) DebugRenderer::Instance()->Enable();
	}	

	void OnKeyInput(const bool* keys, GLfloat dt) 
	{
		if (keys[GLFW_KEY_F1] || keys[GLFW_KEY_B]) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (keys[GLFW_KEY_F2] || keys[GLFW_KEY_N]) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (keys[GLFW_KEY_F3] || keys[GLFW_KEY_M]) glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	void OnKeyDown(int key)
	{
		if (key == GLFW_KEY_P)
		{
			if (!simulationRunning) scene->StartSimulation();
			else scene->StopSimulation();
			simulationRunning = !simulationRunning;
			std::cout << "pause" << std::endl;
		}
		if (key == GLFW_KEY_O)
		{
			if (!debugRendering) DebugRenderer::Instance()->Enable();
			else DebugRenderer::Instance()->Disable();
			debugRendering = !debugRendering;
		}
		if (key == GLFW_KEY_F)
		{
			if (!fullscreen)
				windowManager->SetFullScreen();
			else
				windowManager->SetWindowed();

			fullscreen = !fullscreen;
		}
		if (key == GLFW_KEY_SPACE)
		{
			// ball
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(1,0,0), 15), scene->GetCamera()->GetPosition());
			ball->SetScale(vec3(0.1));
			ball->GetRigidBody()->ApplyLinearMomentum(5.0f * scene->GetCamera()->GetDirection());
			scene->AddEntity(ball);
		}
		// bombardement
		if (key == GLFW_KEY_T)
		{
			vec3 middle = scene->GetCamera()->GetPosition();
			vec3 direction = scene->GetCamera()->GetDirection();
			vec3 right = scene->GetCamera()->GetRight();
			vec3 up = scene->GetCamera()->GetUp();
			
			float deltaX = 1;	// right vector
			float deltaY = 1;	// direction vector
			float deltaZ = 2;	// up vector
			
			int noXdir = 6;
			int noYdir = 6;
			vec3 offset = up*4.f + direction*4.f;
			vec3 startPos = offset + middle - right*(noXdir -1.f)*deltaX/2.f;
			
			for(int i = 0; i < noXdir; ++i)
			{
				for(int j = 0; j < noYdir; ++j)
				{
					vec3 pos = startPos + float(i)*deltaX*right + float(j)*(deltaZ*up+deltaY*direction);
					// ball
					RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(1,0,0), 15), pos);
					ball->SetScale(vec3(0.1));
					//~ ball->GetRigidBody()->ApplyLinearMomentum(5.0f * scene->GetCamera()->GetDirection());
					scene->AddEntity(ball);
				}
			}
		}
		
		// not working very good...
		if (key == GLFW_KEY_Q)
		{
			// spaghetti
			vec3 color(225/255.,196/255.,0/255.);
			
			vec3 middle = scene->GetCamera()->GetPosition();
			vec3 direction = scene->GetCamera()->GetDirection();
			vec3 right = scene->GetCamera()->GetRight();
			float L = 0.2;	// length of single element
			float r = 0.02;	// radius of elements
			float gap = 0.6*r;	// gap between the elements
			
			int noElements = 6;
			vec3 startPos = middle - right*(L*noElements + gap*(noElements-1))/2.f;
			vec3 shift = right*(L+gap);
			RigidBodyModel* oldCylinder;
			quat rot = inverse(scene->GetCamera()->GetViewMatrix());
			vec3 EulerAngles(0, 0, radians(90.));
			quat MyQuaternion = quat(EulerAngles);
			rot = rot*MyQuaternion;
			for(int i = 0; i < noElements; ++i)
			{
				vec3 pos = startPos + float(i)*shift;
				RigidBodyModel* newCylinder;
				newCylinder = new RigidBodyModel(MeshGenerator::CreateCylinder(color), pos);
				newCylinder->SetScale(vec3(r,L,r));
				newCylinder->SetRotation(rot);
				newCylinder->GetRigidBody()->ApplyLinearMomentum(direction*5.f);
				scene->AddEntity(newCylinder);
				if(i != 0)
				{
					std::cout << i << std::endl;
					vec3 jointPos = pos - shift/2.f;
					BallJointConstraint* constraint = new BallJointConstraint(newCylinder->GetRigidBody(), oldCylinder->GetRigidBody(), jointPos);
					scene->GetPhysicManager()->AddConstraint(constraint);
				}
				oldCylinder = newCylinder;
			}
		}
	}

	void OnMouseMoved(GLfloat xpos, GLfloat ypos, GLfloat xoffset, GLfloat yoffset)
	{
	}

	void OnScrollInput(GLfloat xoffset, GLfloat yoffset)
	{
	}
};
