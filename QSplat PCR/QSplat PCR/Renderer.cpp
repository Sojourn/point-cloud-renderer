#include "Renderer.h"
#include "Shader.h"

#ifndef BUFFER_OFFSET
#  define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#endif

//Initialize static members

GLuint Renderer::axesShader = 0;
GLuint Renderer::axesArrayBuffer = 0;
GLuint Renderer::axesVertexArray = 0;
GLuint Renderer::axesViewLocation = 0;
GLuint Renderer::axesProjectionLocation = 0;

GLuint Renderer::splatArrayBuffer = 0;
GLuint Renderer::splatBufferLength = 0;
GLuint Renderer::splatLength = 0;

Renderer::~Renderer(void) {
	if (glIsBuffer(splatArrayBuffer))
		glDeleteBuffers(1, &splatArrayBuffer);

	if (splatVertexArray != NULL)
		glDeleteVertexArrays(1, &splatVertexArray);

	
	if (glIsBuffer(axesArrayBuffer))
		glDeleteBuffers(1, &axesArrayBuffer);

	if (axesVertexArray != NULL)
		glDeleteVertexArrays(1, &axesVertexArray);

}

void Renderer::Initialize(int maxBufferLength) {
	GLuint program = splatter->Initialize();
	glUseProgram(program);
	
	glBindBuffer(GL_ARRAY_BUFFER, splatArrayBuffer);

	glGenVertexArrays(1, &splatVertexArray);
	glBindVertexArray(splatVertexArray);

	///3 float
	int offset= 0;
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	if (vPosition != ((GLuint) -1)) {
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(QSplat), BUFFER_OFFSET(offset));
	}

	///1 float
	offset= sizeof(GLfloat) * 3;
	GLuint vRadius = glGetAttribLocation(program, "vRadius");
	if (vRadius != ((GLuint) -1)) {
		glEnableVertexAttribArray(vRadius);
		glVertexAttribPointer(vRadius, 1, GL_FLOAT, GL_FALSE, sizeof(QSplat), BUFFER_OFFSET(offset));
	}

	///4 unsigned byte
	offset= sizeof(GLfloat) * 4;
	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	if (vNormal != ((GLuint) -1)) {
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_BYTE, GL_TRUE, sizeof(QSplat), BUFFER_OFFSET(offset));
	}


	///4 unsigned byte
	offset= sizeof(GLfloat) * 4+ sizeof(GLbyte) * 3;
	GLuint vNormalCone = glGetAttribLocation(program, "vNormalCone");
	if (vNormalCone != ((GLuint) -1)) {
		glEnableVertexAttribArray(vNormalCone);
		glVertexAttribPointer(vNormalCone, 1, GL_BYTE, GL_FALSE, sizeof(QSplat), BUFFER_OFFSET(offset));
	}


	///4 unsigned byte
	offset= sizeof(GLfloat) * 4 + sizeof(GLbyte) * 4;
	GLuint vColor = glGetAttribLocation(program, "vColor");
	if (vColor != ((GLuint) -1)) {
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QSplat), BUFFER_OFFSET(offset));
	}

	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projectionLocation = glGetUniformLocation(program, "projection");
	lightVectorLocation = glGetUniformLocation(program, "lightVector");


	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
	


}

void Renderer::LoadBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, splatArrayBuffer);
	buffer->Lock(IBuffer::Renderer);
	const std::vector<QSplat> &splatArray = buffer->Array(IBuffer::Renderer);
	splatLength = splatArray.size();
	if(splatLength > 0)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, std::min(splatLength,splatBufferLength) * sizeof(QSplat), &splatArray[0]);
	}
	bufferVersion = buffer->GetVersion();
	buffer->Unlock(IBuffer::Renderer);
}

void Renderer::LoadCameraUniforms() {
	glUseProgram(splatter->getProgram());

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(camera.getView()));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(camera.getProjection()));
	glUniform4fv(lightVectorLocation, 1, value_ptr(lightVector));

	
	glUseProgram(axesShader);
	glUniformMatrix4fv(axesViewLocation, 1, GL_FALSE, value_ptr(camera.getView()));

	glUseProgram(0);
}

///<summary>
/// Render to the current OpenGL Context and Framebuffer
///</summary>
void Renderer::Draw() {
	profiler->FrameStart();

	if (buffer->GetVersion() != bufferVersion)
		LoadBuffer();
	LoadCameraUniforms();
	splatter->Draw(splatVertexArray, splatLength);
	profiler->FrameEnd();
}


void Renderer::DrawAxes() {
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(axesShader);
	glBindVertexArray(axesVertexArray);
	glLineWidth(10);
	glDrawArrays(GL_LINES, 0, 6);
	glLineWidth(1);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Renderer::InitializeAxes() {
	axesShader = InitShader("Shaders\\axis_v.glsl", "Shaders\\axis_f.glsl");
	glUseProgram(axesShader);

	vec3 vertices[2 * 2 * 3];

	//x, red
	vertices[0] = vec3(0,0,0); //pos
	vertices[1] = vec3(1,0,0); //color
	vertices[2] = vec3(1,0,0); //pos
	vertices[3] = vec3(1,0,0); //color

	//y, green
	vertices[4] = vec3(0,0,0); //pos
	vertices[5] = vec3(0,1,0); //color
	vertices[6] = vec3(0,1,0); //pos
	vertices[7] = vec3(0,1,0); //color

	//z, blue
	vertices[8] = vec3(0,0,0); //pos
	vertices[9] = vec3(0,0,1); //color
	vertices[10] = vec3(0,0,1); //pos
	vertices[11] = vec3(0,0,1); //color

	glGenBuffers(1, &axesArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, axesArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2*6 * sizeof(vec3), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &axesVertexArray);
	glBindVertexArray(axesVertexArray);

	///3 float
	GLuint vPosition = glGetAttribLocation(axesShader, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), BUFFER_OFFSET(0));

	///3 float
	GLuint vColor = glGetAttribLocation(axesShader, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), BUFFER_OFFSET(sizeof(vec3)));

	axesViewLocation = glGetUniformLocation(axesShader, "view");
	axesProjectionLocation = glGetUniformLocation(axesShader, "projection");

	
	mat4 proj = ortho(-1.5f,1.5f,-1.5f,1.5f,-10000.0f,10000.0f);

	glUniformMatrix4fv(axesProjectionLocation, 1, GL_FALSE, value_ptr(proj));

	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
}


void Renderer::StaticInitialize(int maxBufferLength) {
	splatBufferLength = maxBufferLength;
	InitializeAxes();

	glGenBuffers(1, &splatArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, splatArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxBufferLength * sizeof(QSplat), NULL, GL_STREAM_DRAW);
	
	glEnable (GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//glEnable(GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
