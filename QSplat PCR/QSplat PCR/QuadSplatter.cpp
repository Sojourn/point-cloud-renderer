#include "QuadSplatter.h"
#include "Shader.h"

QuadSplatter::QuadSplatter(void) : shader(0) {
}


QuadSplatter::~QuadSplatter(void) {
		glDeleteProgram(shader);
}

void QuadSplatter::Draw(GLuint vao, int32_t count) {
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint QuadSplatter::Initialize() {
	shader = InitShader("Shaders\\basic_v.glsl", "Shaders\\quad_g.glsl", "Shaders\\quad_f.glsl");

	return shader;
}
