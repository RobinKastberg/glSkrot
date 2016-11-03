#include "stdafx.h"

#define NUM_PARTICLES 10000
#define WIDTH 5.0
#define HEIGHT 5.0

static GLuint vbo;
static GLuint vao;
static GLuint ubo;
static struct shader_program quadp;



__declspec(align(16)) static float const quad[] = { -1.0f, -1.0f, -0.0f,
1.0f, -1.0f, 0.0f,
1.0f, 1.0f, 0.0f,
-1.0f, 1.0f, 0.0f,

0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
0.0f, 1.0f
};

void init_quad()
{
	glGenVertexArrays(2, &vao);
	glGenBuffers(2, &vbo);



	shader_init(&quadp, "quad");
	shader_source(&quadp, GL_FRAGMENT_SHADER, quad_frag, quad_frag_len);
	shader_source(&quadp, GL_VERTEX_SHADER, standard_vert, standard_vert_len);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)80);
}
#include <omp.h>
void draw_quad()
{
	glBindVertexArray(vao);
	glUseProgram(quadp.program);
	glDrawArrays(GL_QUADS, 0, 4);
}