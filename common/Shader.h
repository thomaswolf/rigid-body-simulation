/*
 * Simple shader helper 
 */
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/*
 * Should be derived to handle platform specific stuff
 */
class Shader
{

public:
	GLuint Program;


    ~Shader()
    {
        glDeleteProgram(Program);
    }

    // Uses the current shader
    void Use() 
    { 
        glUseProgram(this->Program); 
    }
};
