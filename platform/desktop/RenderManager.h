/*
 * Loads shaders and renders a Scene object.
 * Implements multi pass rendering to create shadows.
 * Code based on: http://learnopengl.com/#!Advanced-Lighting/Shadows/Shadow-Mapping
 */ 

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "Scene.h"
#include "DebugRenderer.h"
#include "DesktopWindowManager.h"
#include "DesktopShader.h"

// size of depth map for shadows mapping
#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096


class RenderManager
{

private:

	// the scene we are rendering 
	Scene* scene;

	// window manager with window where our framebuffer is used
	DesktopWindowManager* windowManager;

	// shaders
	DesktopShader colorShader;
	DesktopShader depthShader;
	DesktopShader debugShader;
	
	// shadow depth map
	GLuint depthMapFBO;
	GLuint depthMap;

public:
	RenderManager(DesktopWindowManager* windowManager, Scene* scene) : 
			colorShader("colorVertexShader.txt", "colorFragmentShader.txt"), 
			depthShader("depthVertexShader.txt", "depthFragmentShader.txt"),
			debugShader("debugVertexShader.txt", "debugFragmentShader.txt")

	{
		assert(windowManager != NULL && scene != NULL);

		this->scene = scene;
		this->windowManager = windowManager;
		this->initBuffers();
	}

	void Draw()
	{
		//1st pass: create shadow depth map

		depthShader.Use();

		// use depth map framebuffer
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		renderScene(depthShader);


		//2nd pass: render with shadow mapping

		colorShader.Use();

		// use default framebuffer 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowManager->GetWidth(), windowManager->GetHeight());

		//135-206-250		
		glClearColor(135./255., 206./255., 250./255., 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);

		// bind shadow map for fragment shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap); 

		renderScene(colorShader);

		// debug pass
		debugShader.Use();
		renderDebugInformation(debugShader);



		glfwSwapBuffers(windowManager->GetWindow());
	}

private:

	void renderScene(Shader& shader)
	{
		Light* light = scene->GetLight();
		Camera* camera = scene->GetCamera();

		// get parameter location for the vertex shader
		GLuint viewLoc = glGetUniformLocation(shader.Program, "view");
		GLuint projectionLoc = glGetUniformLocation(shader.Program, "projection");

		// get parameter location for the fragment shader
		GLuint lightPosLoc = glGetUniformLocation(shader.Program, "lightPos");
		GLuint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
		GLuint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
		GLuint lightSpaceMatrixLoc = glGetUniformLocation(shader.Program, "lightSpaceMatrix");
		
		// set camera data
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
		glUniform3fv(viewPosLoc, 1, value_ptr(camera->GetPosition()));

		// set light data
		glUniform3fv(lightPosLoc,1, value_ptr(light->GetPosition()));
		glUniform3fv(lightColorLoc,1, value_ptr(light->GetColor()));
		glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, value_ptr(light->GetLightSpaceMatrix()));
	
		scene->Draw(shader);
	}

	void renderDebugInformation(Shader& shader)
	{
		Light* light = scene->GetLight();
		Camera* camera = scene->GetCamera();

		// get parameter location for the vertex shader
		GLuint viewLoc = glGetUniformLocation(shader.Program, "view");
		GLuint projectionLoc = glGetUniformLocation(shader.Program, "projection");

		// set camera data
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
		DebugRenderer::Instance()->Draw(debugShader);
	}

	void initBuffers()
	{
		// set viewport
		int width, height;
		glfwGetFramebufferSize(windowManager->GetWindow(), &width, &height);
		glViewport(0, 0, width, height);

		// enable depth test and culling
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);  
		glFrontFace(GL_CCW);  

		// framebuffer for shadow depth map
		glGenFramebuffers(1, &depthMapFBO);  

		// texture for shadow depth map
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
					 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);  

		// set shadow outside the map to 0
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  


		// bind shadow depth buffer to texture => the depth buffer will be rendered to the texture
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// multi sampling
		glfwWindowHint(GLFW_SAMPLES, 16);
		glEnable(GL_MULTISAMPLE);  
		
	}
};

