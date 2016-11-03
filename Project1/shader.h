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