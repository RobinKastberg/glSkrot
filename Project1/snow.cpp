#include "stdafx.h"

#define NUM_PARTICLES 10000
#define WIDTH 5.0
#define HEIGHT 5.0

static GLuint vbo[2];
static GLuint vao[2];
static GLuint ubo;
static struct shader_program quadp;

static GLuint out_vbo = 1;

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
void init_snow()
{
	glGenVertexArrays(2, &vao[0]);
	glGenBuffers(2, &vbo[0]);



	shader_init(&quadp, "quad");
	const GLchar* feedbackVaryings[] = { "out_Position" };
	glTransformFeedbackVaryings(quadp.program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	shader_source(&quadp, GL_FRAGMENT_SHADER, snow_frag, snow_frag_len);
	shader_source(&quadp, GL_VERTEX_SHADER, snow_vert, snow_vert_len);
	glUniformBlockBinding(quadp.program, glGetUniformBlockIndex(quadp.program, "global"), 0);
#pragma omp parallel for num_threads(2)
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		parts.positions[i].x = WIDTH*((float)rand()) / RAND_MAX - WIDTH / 2;
		parts.positions[i].y = WIDTH*((float)rand()) / RAND_MAX - WIDTH / 2;
		parts.positions[i].z = HEIGHT*((float)rand()) / RAND_MAX;
		parts.positions[i].w = 1.0f;
	}
	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(parts.positions), parts.positions, GL_STREAM_COPY);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	}
}

void draw_snow()
{
	glBindVertexArray(vao[out_vbo]);
	shader_use(&quadp);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo[1 - out_vbo]); // TODO: Why does this make it go faster?
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[1 - out_vbo]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glEndTransformFeedback();

	out_vbo = (out_vbo + 1) % 2;
}