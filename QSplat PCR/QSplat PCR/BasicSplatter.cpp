#include "BasicSplatter.h"
#include "Shader.h"

BasicSplatter::BasicSplatter(void) : shader(0) {
}


BasicSplatter::~BasicSplatter(void) {
		glDeleteProgram(shader);
}

void BasicSplatter::Draw(GLuint vao, int32_t count) {
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint BasicSplatter::Initialize() {
	shader = InitShader("Shaders\\basic_v.glsl", "Shaders\\basic_g.glsl", "Shaders\\basic_f.glsl");

	return shader;
}
