/*
 * Abstracts the GLFwindow
 */
#pragma once

#include <GL/glew.h>

#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "WindowManager.h"

#define WIDTH 1280
#define HEIGHT 1024

class DesktopWindowManager : public WindowManager
{

private:
	GLFWwindow* window;
	bool fullscreen = false;

public:

	DesktopWindowManager(bool fullscreen, int width = -1, int height = -1)
	{
		this->width = width;
		this->height = height;
		this->fullscreen = fullscreen;
	}
	
	~DesktopWindowManager()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool IsWindowRunning()
	{
		return !glfwWindowShouldClose(window);
	}

	void SetFullScreen()
	{
		std::cout << "set fullscreen mode" << std::endl;

		auto monitor = glfwGetPrimaryMonitor();

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		width = mode->width;
		height = mode->height;
		glfwSetWindowMonitor(window, NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
	}	

	void SetWindowed()
	{
		std::cout << "set window mode" << std::endl;
		width = WIDTH;
		height = HEIGHT;
		auto monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(window, NULL, (mode->width- WIDTH)/2, (mode->height-HEIGHT)/2, width, height, mode->refreshRate);
	}

	bool CreateWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// create window
		if (fullscreen)
		{
			auto monitor = glfwGetPrimaryMonitor();

			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			if (width == -1)	width = mode->width;
			if (height == -1) 	height = mode->height;

			std::cout << width << " " << height << std::endl;

			this->window = glfwCreateWindow(mode->width, mode->height, "PBS Sim", monitor, NULL);
		}
		else
		{
			if (width == -1)	width = WIDTH;
			if (height == -1) 	height = HEIGHT;
			this->window = glfwCreateWindow(WIDTH, HEIGHT, "PBS Sim", NULL, NULL);
		}


		//this->window = glfwCreateWindow(WIDTH, HEIGHT, "Phys-Sim", monitor, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return false;
		}
		
		// set opengl context to window
		glfwMakeContextCurrent(window);

		// init glew (handles graphics interface automatically)
		glewExperimental = GL_TRUE;
		glewInit();

		return true;
	}

	GLFWwindow* GetWindow()
	{
		return this->window;
	}


};
