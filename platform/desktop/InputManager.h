/*
* Abstracts the opengl intput events
*/
#pragma once

#include "DesktopWindowManager.h"

#include <GL/glew.h>
#include <list>

class InputProcessor
{
public:
	virtual void OnMouseMoved(GLfloat xpos, GLfloat ypos, GLfloat xoffset, GLfloat yoffset) {}
	virtual void OnScrollInput(GLfloat xoffset, GLfloat yoffset) {}
	virtual void OnKeyInput(const bool* keys, GLfloat dt) {}
	virtual void OnKeyDown(const int key) {}
	virtual void OnKeyUp(const int key) {}
};


class InputManager
{
private:

	static std::list<InputProcessor*> listeners;
	static bool keys[1024]; // pressed keys in next update
	static GLfloat lastX;
	static GLfloat lastY;
	static bool firstMouse;

public:

	InputManager(DesktopWindowManager* window)
	{
		this->initInput(window->GetWindow());
	}

	~InputManager()
	{
		for (InputProcessor* p : listeners)
		{
			delete p;
		}
	}

	void RegisterInputProcessor(InputProcessor* p)
	{
		listeners.push_back(p);	
	}

	void RemoveInputProcessor(InputProcessor *p)
	{
		listeners.remove(p);
	}

	void Update(GLfloat dt)
	{
		glfwPollEvents();

		for (InputProcessor* p : listeners)
		{
			p->OnKeyInput(keys, dt);
		}
	}

private:

	void initInput(GLFWwindow* window)
	{
		// Set the required callback functions
		glfwSetKeyCallback(window, this->key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}



	// Is called whenever a key is pressed/released via GLFW
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
			{
				keys[key] = true;

				for (InputProcessor* p : listeners)
				{
					p->OnKeyDown(key);
				}
			}
			else if (action == GLFW_RELEASE)
			{
				keys[key] = false;

				for (InputProcessor* p : listeners)
				{
					p->OnKeyUp(key);
				}
			}
		}
	}


	static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		// calculate mouse position difference
		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left

		for (InputProcessor* p : listeners)
		{
			p->OnMouseMoved(xpos, ypos, xoffset, yoffset);
		}
		lastX = xpos;
		lastY = ypos;
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		for (InputProcessor* p : listeners)
		{
			p->OnScrollInput(xoffset, yoffset);
		}
	}
};

// init static
bool InputManager::firstMouse = true;
std::list<InputProcessor*> InputManager::listeners;
bool InputManager::keys[1024];
		 
GLfloat InputManager::lastX = DesktopWindowManager::GetWidth() / 2.;
GLfloat InputManager::lastY = DesktopWindowManager::GetHeight() / 2.;
