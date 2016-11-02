#pragma once
#include "stdafx.h"
struct shader_program {
	GLuint program = 0;
	GLuint frag_shader = 0;
	GLuint vert_shader = 0;
};

void shader_init(struct shader_program *self);
bool shader_source(struct shader_program *self, GLenum type, const unsigned char * const str, int size);
void shader_verify(const struct shader_program *self);