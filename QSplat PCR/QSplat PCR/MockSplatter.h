#pragma once
#include "ISplatter.h"

class MockSplatter : public ISplatter {
private:
	GLuint shader;

public:

	MockSplatter(void) : shader(0) {
	}

	~MockSplatter(void) {
		glDeleteProgram(shader);
	}

	///<summary>
	/// Get the need OpenGL Major Version
	///</summary>
	virtual int GetOpenGLMajorVersion() {
		return 3;
	}

	///<summary>
	/// Get the need OpenGL Minor Version
	///</summary>
	virtual int GetOpenGLMinorVersion() {
		return 3;
	}
	
	
	///<summary>
	/// Render to the current OpenGL Context and Framebuffer.
	///</summary>
	/// <param name="buffer">The GL_ARRAY_BUFFER where the splats are stored</param>
	/// <param name="count">The number of splats to be drawn</param>
	virtual void Draw(GLuint vao, int32_t count);

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

};

