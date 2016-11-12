#pragma once
#include "stdafx.h"
struct meshData {
	vec3 vertex;
	vec3 normal;
	vec2 texcoord;
};
struct mesh {
	struct object object;
	GLuint vbo;
	GLuint ibo;
	GLuint vao;
	GLuint to;
	struct shader_program sp;
	struct meshData *data;
	struct perModel *model;
	unsigned int uniformIndex;
	unsigned int numVerts;
	unsigned int *indices;
	unsigned int numIndices;
	bool wireframe;
};
void mesh_new(struct mesh *self, unsigned int numVerts, unsigned int numIndices);
void mesh_prepare(struct mesh *self);
void mesh_draw(struct mesh *self, GLenum type, unsigned int count, unsigned int offset);