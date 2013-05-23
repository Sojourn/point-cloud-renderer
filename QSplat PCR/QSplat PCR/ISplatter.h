#pragma once
#include "QSplat.h"
#include <gl\glew.h>

class ISplatter
{
public:

	///<summary>
	/// Get the need OpenGL Major Version
	///</summary>
	virtual int GetOpenGLMajorVersion() = 0;

	///<summary>
	/// Get the need OpenGL Minor Version
	///</summary>
	virtual int GetOpenGLMinorVersion() = 0;
	
	///<summary>
	/// Initialize needed resources.
	///</summary>
	virtual GLuint Initialize() = 0;

	///<summary>
	/// Get the shader program associated with drawing the splats
	///</summary>
	virtual GLuint getProgram() = 0;

	///<summary>
	/// Render to the current OpenGL Context and Framebuffer.
	///</summary>
	/// <param name="vao">The GL_ARRAY_BUFFER where the splats are stored</param>
	/// <param name="count">The number of splats to be drawn</param>
	virtual void Draw(GLuint vao, int32_t count) = 0;
};

