#include "stdafx.h"

#define NUM_PARTICLES 10000
#define WIDTH 20.0
#define HEIGHT 20.0
static GLuint vbo[2];
static GLuint vao;
static GLuint ubo;
static struct shader_program quadp;

static GLuint out_vbo = 1;
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
	glGenBuffers(2, &vbo[0]);
	glBindVertexArray(vao);


	shader_init(&quadp, "quad");
	const GLchar* feedbackVaryings[] = { "out_Position" };
	glTransformFeedbackVaryings(quadp.program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(parts.positions), parts.positions, GL_DYNAMIC_COPY);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo[1]);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(parts.positions), NULL, GL_DYNAMIC_COPY);

	
}
#include <omp.h>
void draw_quad()
{
	glEnable(GL_PROGRAM_POINT_SIZE);
//	glEnable(GL_RASTERIZER_DISCARD);
	glBindVertexArray(vao);
	glUseProgram(quadp.program);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1-out_vbo]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo[out_vbo]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[out_vbo]);

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glEndTransformFeedback();

	out_vbo = (out_vbo + 1) % 2;
}