/*
* Abstracts the opengl intput events
*/
#pragma once


#include "AndroidWindowManager.h"
#include <list>

class InputProcessor
{
public:
	virtual void OnTouch(int iPointerID, float fPosX, float fPosY, int iAction) {}
    virtual void Update(double dt) {}
	virtual ~InputProcessor() {}
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

	InputManager(WindowManager* window)
	{
		this->initInput();
	}

	~InputManager()
	{
		for (InputProcessor* p : listeners)
		{
			delete p;
		}
		listeners.clear();
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
		//glfwPollEvents();

		for (InputProcessor* p : listeners)
		{
			p->Update(dt);
		}
	}

	void OnTouch(int iPointerID, float fPosX, float fPosY, int iAction)
	{
		for (InputProcessor* p : listeners)
		{
		    // we put the origin in the bottom left corner
			p->OnTouch(iPointerID,
			fPosX / WindowManager::GetWidth(),
			1 - (fPosY / WindowManager::GetHeight()),
			iAction);
		}
	}

private:

	void initInput()
	{
		// Set the required callback functions

	}



	// Is called whenever a key is pressed/released via GLFW
	/*static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
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
	}*/
};

// init static
bool InputManager::firstMouse = true;
std::list<InputProcessor*> InputManager::listeners;
bool InputManager::keys[1024];
