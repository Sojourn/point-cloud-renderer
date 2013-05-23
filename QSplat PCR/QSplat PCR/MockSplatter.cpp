#include "MockSplatter.h"
#include "Shader.h"

void MockSplatter::Draw(GLuint vao, int32_t count) {
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint MockSplatter::Initialize() {
	shader = InitShader("Shaders\\mock_v.glsl", "Shaders\\mock_g.glsl", "Shaders\\mock_f.glsl");
	return shader;
}

