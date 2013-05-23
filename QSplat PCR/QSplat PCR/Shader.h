#pragma once
#include "GL/glew.h"

///Code taken from

//////////////////////////////////////////////////////////////////////////////
//
//  --- Angel.h ---
//
//   The main header file for all examples from Angel 6th Edition
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BUFFER_OFFSET
#  define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#endif



// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

// Create a GLSL program object from vertex, geometry, and fragment shader files
GLuint InitShader(const char* vShaderFile, const char* gShaderFile, const char* fShaderFile);
