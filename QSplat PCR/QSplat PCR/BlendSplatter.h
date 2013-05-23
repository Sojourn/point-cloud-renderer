#pragma once
#include "ISplatter.h"
#include "glmconfig.h"
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

class BlendSplatter : public ISplatter {
private:
	int type;

	GLuint shader_linked;
	GLuint shader_resolver;
	GLuint atomic_counter;
	
	GLuint head;
	GLuint next;
	GLuint depth;
	GLuint color;
	
	//Max Size [width, height, storage width, storage height]
	ivec4 windowSize; 

	GLuint rectArrayBuffer;
	GLuint rectVertexArray;

	inline void setupTextures();
	inline void tearDownTextures();
	inline void linkedTextures(GLuint shader);
	inline void initializeRectangle();


public:
	BlendSplatter(int width, int height, int type);
	~BlendSplatter(void);

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
		return shader_linked;
	}

	///<summary>
	/// Render to the current OpenGL Context and Framebuffer.
	///</summary>
	/// <param name="vao">The GL_ARRAY_BUFFER where the splats are stored</param>
	/// <param name="count">The number of splats to be drawn</param>
	virtual void Draw(GLuint vao, int32_t count);
};

