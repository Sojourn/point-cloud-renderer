#include "QuadSplatter_PC.h"
#include "Shader.h"

QuadSplatter_PC::QuadSplatter_PC(void) : shader(0) {
}


QuadSplatter_PC::~QuadSplatter_PC(void) {
		glDeleteProgram(shader);
}

void QuadSplatter_PC::Draw(GLuint vao, int32_t count) {
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint QuadSplatter_PC::Initialize() {
	shader = InitShader("Shaders\\basic_v.glsl", "Shaders\\quad_pc_g.glsl", "Shaders\\quad_pc_f.glsl");

	return shader;
}

