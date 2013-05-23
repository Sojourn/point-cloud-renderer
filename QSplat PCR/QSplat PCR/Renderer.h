#pragma once
#include <gl\glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "IRenderer.h"
#include "IBuffer.h"
#include "ISplatter.h"
#include "Profiler.h"
#include "Camera.h"

using namespace std;
using namespace glm;

class Renderer : public IRenderer {

private:
	pBuffer buffer;
	pSplatter splatter;
	pProfiler profiler;
	Camera &camera;

	static GLuint axesShader;
	static GLuint axesArrayBuffer;
	static GLuint axesVertexArray;
	static GLuint axesViewLocation;
	static GLuint axesProjectionLocation;

	static GLuint splatArrayBuffer;
	static GLuint splatBufferLength;
	static GLuint splatLength;

	GLuint splatVertexArray;
	
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;
	GLuint lightVectorLocation;
	
	mat4 &model;
	vec4 &lightVector;

	int bufferVersion;

	void LoadBuffer();
	void LoadCameraUniforms();

	static void InitializeAxes();

public:
	Renderer(pBuffer buffer, pSplatter splatter, pProfiler profiler, mat4 &model, vec4 &lightVector) : 
		buffer(buffer), splatter(splatter), profiler(profiler), splatVertexArray(NULL), 
		model(model), lightVector(lightVector), camera(Camera::inst()), bufferVersion(-1) {
	}

	~Renderer(void);

	///<summary>
	/// Get the need OpenGL Major Version
	///</summary>
	virtual int GetOpenGLMajorVersion() {
		return splatter->GetOpenGLMajorVersion();
	}

	///<summary>
	/// Get the need OpenGL Minor Version
	///</summary>
	virtual int GetOpenGLMinorVersion() {
		return splatter->GetOpenGLMinorVersion();
	}

	///<summary>
	/// Render to the current OpenGL Context and Framebuffer
	///</summary>
	virtual void Draw();

	///<summary>
	/// Clears the depth and draws a set of axis at world zero
	///</summary>
	virtual void DrawAxes();

	///<summary>
	/// Initialize needed resources.
	///</summary>
	///<param name="maxBufferLength">Ignored</param>
	virtual void Initialize(int maxBufferLength);

	///<summary>
	/// Initialize shared resources.
	///</summary>
	///<param name="maxBufferLength">The size of the OpenGL Buffer to be allocated</param>
	static void StaticInitialize(int maxBufferLength);

};

