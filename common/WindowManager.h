#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

/*
 *  Should be derived to handle platform specific stuff
 */
class WindowManager
{

protected:
	static GLfloat width;
	static GLfloat height;

public:

	static GLfloat GetWidth() { return width; };
	static GLfloat GetHeight() { return height; };
};

GLfloat WindowManager::width = 0;
GLfloat WindowManager::height = 0;
