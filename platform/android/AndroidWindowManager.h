#pragma once

#include "AndroidWindowManager.h"

using namespace glm;

class AndroidWindowManager : public WindowManager
{


public:



	AndroidWindowManager ()
	{

	}
	
	~AndroidWindowManager ()
	{

	}

	void Resize(int width, int height)
	{
		WindowManager::width = width;
		WindowManager::height = height;
	}

	bool CreateWindow() {

	}

};
