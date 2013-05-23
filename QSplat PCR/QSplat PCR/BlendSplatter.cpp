#include "BlendSplatter.h"
#include "Shader.h"

BlendSplatter::BlendSplatter(int width, int height, int type = 0) : 
	shader_linked(0), shader_resolver(0), atomic_counter(0), type(type) {
		windowSize[0] = width;
		windowSize[1] = height;
		windowSize[2] = 4*width;
		windowSize[3] = 4*height;
}


BlendSplatter::~BlendSplatter(void) {
		glDeleteProgram(shader_linked);
		glDeleteProgram(shader_resolver);
		if (glIsBuffer(atomic_counter))
			glDeleteBuffers(1, &atomic_counter);
		if (glIsTexture(head))
			glDeleteTextures(1, &head);
		if (glIsTexture(next))
			glDeleteTextures(1, &next);
		if (glIsTexture(depth))
			glDeleteTextures(1, &depth);
		if (glIsTexture(color))
			glDeleteTextures(1, &color);

		
	
	if (glIsBuffer(rectArrayBuffer))
		glDeleteBuffers(1, &rectArrayBuffer);

	if (rectVertexArray != NULL)
		glDeleteVertexArrays(1, &rectVertexArray);
}

void BlendSplatter::Draw(GLuint vao, int32_t count) {
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_BLEND);

	setupTextures();

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomic_counter);
	
	glUseProgram(shader_linked);
	glBindVertexArray(vao);
	linkedTextures(shader_linked);

	
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glDepthMask(GL_FALSE);

	glDrawArrays(GL_POINTS, 0, count);

	
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glDepthMask(GL_TRUE);
	
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shader_resolver);
	glBindVertexArray(rectVertexArray);
	linkedTextures(shader_resolver);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	tearDownTextures();

	//Reset Atomic Counter	
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter);
	GLuint temp = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),&temp);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	//glDisable(GL_BLEND);
}

GLuint BlendSplatter::Initialize() {
	switch (type) {
	case 1:
		shader_linked = InitShader("Shaders\\blend_v.glsl", "Shaders\\mock_g.glsl", "Shaders\\blend_linked_f.glsl");
		break;
	case 2:
		shader_linked = InitShader("Shaders\\blend_v.glsl", "Shaders\\quad_g.glsl", "Shaders\\blend_quad_linked_f.glsl");
		break;
	default:
		shader_linked = InitShader("Shaders\\blend_v.glsl", "Shaders\\basic_g.glsl", "Shaders\\blend_linked_f.glsl");
		break;
	}
	shader_resolver = InitShader("Shaders\\blend_resolve_v.glsl",  "Shaders\\blend_resolve_f.glsl");

	//Create Atomic Counter
	glGenBuffers(1, &atomic_counter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	
	GLuint temp = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),&temp);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	//Create Head Buffer
	GLint *initData = new GLint[windowSize[0]*windowSize[1]];
	std::fill_n(initData, windowSize[0]*windowSize[1], -1);

	glGenTextures(1, &head);
	glBindTexture(GL_TEXTURE_RECTANGLE, head);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32I, windowSize[0], windowSize[1], 0, GL_RED_INTEGER, GL_INT, initData);

	
	//Create Next Buffer
	glGenTextures(1, &next);
	glBindTexture(GL_TEXTURE_RECTANGLE, next);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32I, windowSize[2], windowSize[3], 0, GL_RED_INTEGER, GL_INT, NULL);

	//Create Depth Buffer
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_RECTANGLE, depth);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32F , windowSize[2], windowSize[3], 0, GL_RED, GL_FLOAT, NULL);
	
	//Create Color Buffer
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_RECTANGLE, color);
	//glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F , windowSize[2], windowSize[3], 0, GL_RGBA, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32UI , windowSize[2], windowSize[3], 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	
	delete initData;

	initializeRectangle();

	
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return shader_linked;
}

inline void BlendSplatter::setupTextures() {
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, color);
	//glBindImageTexture(0, color, 0, GL_FALSE, 0,  GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(0, color, 0, GL_FALSE, 0,  GL_READ_WRITE, GL_R32UI);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, depth);
	glBindImageTexture(1, depth, 0, GL_FALSE, 0,  GL_READ_WRITE, GL_R32F);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, next);
	glBindImageTexture(2, next, 0, GL_FALSE, 0,  GL_READ_WRITE, GL_R32I);

	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, head);
	glBindImageTexture(3, head, 0, GL_FALSE, 0,  GL_READ_WRITE, GL_R32I);
}

inline void BlendSplatter::tearDownTextures() {
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

}

inline void BlendSplatter::linkedTextures(GLuint shader) {
	glUniform1i(glGetUniformLocation(shader, "image0"), 0 );
	glUniform1i(glGetUniformLocation(shader, "image1"), 1 );
	glUniform1i(glGetUniformLocation(shader, "image2"), 2 );
	glUniform1i(glGetUniformLocation(shader, "image3"), 3 );

	GLint *ptr =  value_ptr(windowSize);

	glUniform4iv(glGetUniformLocation(shader, "vImgSize"), 1, value_ptr(windowSize));
}

inline void BlendSplatter::initializeRectangle() {
	vec3 vertices[2 * 3];

	//triangle 1
	vertices[0] = vec3(-1,-1,0);
	vertices[1] = vec3(1,-1,0);
	vertices[2] = vec3(-1,1,0);

	//triangle 2
	vertices[3] = vec3(1,1,0);
	vertices[4] = vec3(-1,1,0);
	vertices[5] = vec3(1,-1,0);

	glGenBuffers(1, &rectArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rectArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &rectVertexArray);
	glBindVertexArray(rectVertexArray);

	///3 float
	GLuint vPosition = glGetAttribLocation(shader_resolver, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), BUFFER_OFFSET(0));


	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
}

