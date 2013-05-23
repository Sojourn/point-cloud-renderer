#include "BlendSplatterPreSortFrag.h"
#include "Shader.h"

BlendSplatterPreSortFrag::BlendSplatterPreSortFrag(int width, int height, int type = 0) : 
	shader_linked(0), shader_resolver(0), type(type) {
		windowSize[0] = width;
		windowSize[1] = height;
		windowSize[2] = 4*width;
		windowSize[3] = 4*height;
}


BlendSplatterPreSortFrag::~BlendSplatterPreSortFrag(void) {
		glDeleteProgram(shader_linked);
		glDeleteProgram(shader_resolver);
		if (glIsTexture(depth))
			glDeleteTextures(1, &depth);
		if (glIsTexture(color))
			glDeleteTextures(1, &color);

		
	
	if (glIsBuffer(rectArrayBuffer))
		glDeleteBuffers(1, &rectArrayBuffer);

	if (rectVertexArray != NULL)
		glDeleteVertexArrays(1, &rectVertexArray);
}

void BlendSplatterPreSortFrag::Draw(GLuint vao, int32_t count) {
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_BLEND);

	setupTextures();
	
	glUseProgram(shader_linked);
	glBindVertexArray(vao);
	linkedTextures(shader_linked);

	
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glDepthMask(GL_FALSE);

	glDrawArrays(GL_POINTS, 0, count);

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glDepthMask(GL_TRUE);
	

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	glUseProgram(shader_resolver);
	glBindVertexArray(rectVertexArray);
	linkedTextures(shader_resolver);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	tearDownTextures();


	glBindVertexArray(0);
	glUseProgram(0);
	//glDisable(GL_BLEND);
}

GLuint BlendSplatterPreSortFrag::Initialize() {
	shader_linked = InitShader("Shaders\\blend_v.glsl", "Shaders\\blend_quad_sort_g.glsl", "Shaders\\blend_quad_sort_f.glsl");
	shader_resolver = InitShader("Shaders\\blend_resolve_v.glsl",  "Shaders\\blend_sort_resolve_f.glsl");

	//Create Head Buffer
	GLint *initData = new GLint[windowSize[2]*windowSize[3]];
	std::fill_n(initData, windowSize[2]*windowSize[3], 0xFFFFFFFF);


	//Create Depth Buffer
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_RECTANGLE, depth);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32UI , windowSize[2], windowSize[3], 0, GL_RED_INTEGER, GL_UNSIGNED_INT, initData);
	
	
	std::fill_n(initData, windowSize[2]*windowSize[3], 0);

	//Create Color Buffer
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_RECTANGLE, color);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32UI , windowSize[2], windowSize[3], 0, GL_RED_INTEGER, GL_UNSIGNED_INT, initData);
	
	delete initData;

	initializeRectangle();
	
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return shader_linked;
}

inline void BlendSplatterPreSortFrag::setupTextures() {
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, color);
	glBindImageTexture(0, color, 0, GL_FALSE, 0,  GL_READ_WRITE, GL_R32UI);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, depth);
	glBindImageTexture(1, depth, 0, GL_FALSE, 0,  GL_READ_WRITE, GL_R32UI);

}

inline void BlendSplatterPreSortFrag::tearDownTextures() {
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);


}

inline void BlendSplatterPreSortFrag::linkedTextures(GLuint shader) {
	glUniform1i(glGetUniformLocation(shader, "colorImage"), 0 );
	glUniform1i(glGetUniformLocation(shader, "depthImage"), 1 );

	GLint *ptr =  value_ptr(windowSize);

	glUniform4iv(glGetUniformLocation(shader, "vImgSize"), 1, value_ptr(windowSize));
}

inline void BlendSplatterPreSortFrag::initializeRectangle() {
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

