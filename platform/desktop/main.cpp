//#undef NDEBUG

#include <iostream>

#include <GL/glew.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "Scene.h"
#include "Mesh.h"
#include "Camera.h"
#include "CameraInputProcessor.h"
#include "DebugInputProcessor.h"
#include "RigidBody.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "SceneChangeInputProcessor.h"
#include "DesktopWindowManager.h"


#define FPS 60
#define UPDATE_TIME 1./60.

int main(int argc, char** argv)
{
	bool fullscreen = false;
	int width = -1;
	int height = -1;
	if (argc >= 2 && strcmp(argv[1], "fullscreen") == 0)
	{
		fullscreen = true;
		std::cout << argv[1] << std::endl;
		if (argc >= 4)
		{
			width = atoi(argv[2]);
			height = atoi(argv[3]);
		}
	}

	// create window
	DesktopWindowManager* windowManager = new DesktopWindowManager(fullscreen, width, height);
	if (!windowManager->CreateWindow()) return -1;


	// add scene
	Scene* scene = new Scene();

	// add renderer
	RenderManager* renderManager = new RenderManager(windowManager, scene);

	// add input manager
	InputManager* inputManager = new InputManager(windowManager);


	// debug input
	DebugInputProcessor* debugInput = new DebugInputProcessor(windowManager, scene);
	inputManager->RegisterInputProcessor(debugInput);

	Camera* camera = new Camera(vec3(0.0f, 1.0f, 3.0f));
	scene->SetCamera(camera);

	// camera input
	CameraInputProcessor* cameraInput = new CameraInputProcessor(camera);
	inputManager->RegisterInputProcessor(cameraInput);

	// scene change input
	SceneChangeInputProcessor* sceneChangeInput = new SceneChangeInputProcessor(scene, cameraInput);
	inputManager->RegisterInputProcessor(sceneChangeInput);


	double deltaTime = 0.0f;	// Time between current frame and last frame
	double lastFrame = 0.0f;  	// Time of last frame
	double accumulatedTime = 0;

	while(windowManager->IsWindowRunning())
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;  
		accumulatedTime += deltaTime;


		if (accumulatedTime >= UPDATE_TIME)
		{
			// update
			inputManager->Update(UPDATE_TIME);
			scene->Update(UPDATE_TIME);
			accumulatedTime = 0;

			// draw
			renderManager->Draw();
		}
	}

	delete scene;
	delete renderManager;
	delete inputManager; // deletes also all processors
	delete windowManager;

	return 0;
}
