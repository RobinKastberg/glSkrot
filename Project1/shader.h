#pragma once
#include "stdafx.h"
struct shader_program {
	GLuint program = 0;
	GLuint frag_shader = 0;
	GLuint vert_shader = 0;
	const char * name;
};

void shader_init(struct shader_program * self, const char * name);
bool shader_source(struct shader_program *self, GLenum type, const unsigned char * str, int size);
void shader_verify(const struct shader_program *self);
void shader_use(const struct shader_program *self);

#define _SHADER(object, name, vertShader, fragShader) \
	shader_init(&object, name); \
	shader_source(&object, GL_FRAGMENT_SHADER, ##fragShader##_frag, ##fragShader##_frag_len);\
	shader_source(&object, GL_VERTEX_SHADER, ##vertShader##_vert, ##vertShader##_vert_len);
#define SHADER(object, name, vertShader, fragShader) _SHADER(object, name, vertShader, fragShader)

