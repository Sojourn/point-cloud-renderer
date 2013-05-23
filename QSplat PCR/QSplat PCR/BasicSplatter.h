#pragma once
#include "ISplatter.h"

class BasicSplatter : public ISplatter {
private:
	GLuint shader;

public:
	BasicSplatter(void);
	~BasicSplatter(void);

	///<summary>
	/// Get the need OpenGL Major Version
	///</summary>
	virtual int GetOpenGLMajorVersion() {
		return 4;
	}

	///<summary>
	/// Get the need OpenGL Minor Version
	///</summary>
	virtual int GetOpenGLMinorVersion() {
		return 0;
	}
	
	///<summary>
	/// Initialize needed resources.
	///</summary>
	virtual GLuint Initialize();

	///<summary>
	/// Get the shader program associated with drawing the splats
	///</summary>
	virtual GLuint getProgram() {
		return shader;
	}

	///<summary>
	/// Render to the current OpenGL Context and Framebuffer.
	///</summary>
	/// <param name="vao">The GL_ARRAY_BUFFER where the splats are stored</param>
	/// <param name="count">The number of splats to be drawn</param>
	virtual void Draw(GLuint vao, int32_t count);
};

