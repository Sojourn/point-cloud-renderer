#pragma once
#include "QSplat.h"

class IRenderer {
public:

	///<summary>
	/// Render to the current OpenGL Context and Framebuffer
	///</summary>
	virtual void Draw() = 0;

	///<summary>
	/// Clears the depth and draws a set of axis at world zero
	///</summary>
	virtual void DrawAxes() = 0;

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
	///<param name="maxBufferLength">The size of the OpenGL Buffer to be allocated</param>
	virtual void Initialize(int maxBufferLength) = 0;

};

