#include "stdafx.h"
#pragma once
#include "object.h"

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
	unsigned int numVerts;
	unsigned int *indices;
	unsigned int numIndices;
	bool wireframe;
};
struct quad {
	struct mesh mesh;
	unsigned int width;
	unsigned int height;
	unsigned int max_lod;
	float lod;
};

//void quad_new(struct quad *self, unsigned int lod);
void quad_new(struct quad *self, unsigned int max_lod, float xoff = 0.0f, float yoff = 0.0f);

void quad_draw(struct quad *self);

void init_skybox();
void draw_skybox();