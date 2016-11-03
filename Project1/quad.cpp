#include "stdafx.h"

#define NUM_PARTICLES 10000
#define WIDTH 20.0
#define HEIGHT 20.0
static GLuint vbo;
static GLuint vao;
static GLuint ubo;
static struct shader_program quadp;

#include "shaders.h"

__declspec(align(16)) static float const quad[] = { -1.0f, -1.0f, 0.0f,
1.0f, -1.0f, 0.0f,
1.0f, 1.0f, 0.0f,
-1.0f, 1.0f, 0.0f,

0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
0.0f, 1.0f
};
 struct particles
{
	float scale; float pad[3];
	vec4 positions[NUM_PARTICLES];
} parts;
void init_quad()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(parts.positions), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	shader_init(&quadp, "quad");
	shader_source(&quadp, GL_FRAGMENT_SHADER, quad_frag, quad_frag_len);
	shader_source(&quadp, GL_VERTEX_SHADER, quad_vert, quad_vert_len);
	glUniformBlockBinding(quadp.program, glGetUniformBlockIndex(quadp.program, "global"), 0);
#pragma omp parallel for num_threads(2)
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		parts.positions[i].x = globals.cameraPosition.x + WIDTH*((float)rand()) / RAND_MAX - WIDTH / 2;
		parts.positions[i].y = globals.cameraPosition.y + WIDTH*((float)rand()) / RAND_MAX - WIDTH / 2;
		parts.positions[i].z = HEIGHT*((float)rand()) / RAND_MAX;
		parts.positions[i].w = 1.0f;
	}
}
#include <omp.h>
void draw_quad()
{
	glEnable(GL_PROGRAM_POINT_SIZE);
	glBindVertexArray(vao);
	glUseProgram(quadp.program);
//#pragma omp parallel for
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		parts.positions[i].x += 0.003*((rand() % 3) - 1);
		parts.positions[i].y += 0.003*((rand() % 3) - 1);
		parts.positions[i].z += 0.0025*((rand() % 3) - 1) - globals.deltaTime;
		if (parts.positions[i].z < 0) {
			parts.positions[i].z = HEIGHT;
			parts.positions[i].x = globals.cameraPosition.x + WIDTH*((float)rand()) / RAND_MAX - WIDTH / 2;
			parts.positions[i].y = globals.cameraPosition.y + WIDTH*((float)rand()) / RAND_MAX - WIDTH / 2;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(parts.positions), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(parts.positions), &parts.positions);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
}