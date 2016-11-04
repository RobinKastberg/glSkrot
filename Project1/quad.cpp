#include "stdafx.h"

#define WIDTH 100
#define HEIGHT 100

struct meshData {
	vec3 vertex;
	vec3 normal;
	vec2 texcoord;
};

struct meshData data[WIDTH*HEIGHT * 4];

static GLuint vbo;
static GLuint vao;
static GLuint ubo;
static GLuint to;
static struct shader_program quadp;

static float textureBuffer[512*512];

static float const quad[] = { 
-1.0f, -1.0f, 0.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f,

1.0f, -1.0f, 0.0f,
0.0f, 0.0f, 1.0f,
1.0f, 0.0f,

1.0f, 1.0f, 0.0f,
0.0f, 0.0f, 1.0f,
1.0f, 1.0f,

-1.0f, 1.0f, 0.0f,
0.0f, 0.0f, 1.0f,
0.0f, 1.0f
};

void init_quad()
{
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			memcpy(&data[4 * i*WIDTH + 4 * j], quad, sizeof(quad));
			for (int k = 0; k < 4; k++)
			{
				data[4 * i*WIDTH + 4 * j].vertex.x += 0.5*i;
				data[4 * i*WIDTH + 4 * j].vertex.y += 0.5*j;
				data[4 * i*WIDTH + 4 * j].texcoord.x = (float)i/WIDTH;
				data[4 * i*WIDTH + 4 * j].texcoord.y = (float)j/HEIGHT;

				data[4 * i*WIDTH + 4 * j + 1].vertex.x += 0.5*i;
				data[4 * i*WIDTH + 4 * j + 1].vertex.y += 0.5*j;
				data[4 * i*WIDTH + 4 * j + 1].texcoord.x = (float)(i+1) / WIDTH;
				data[4 * i*WIDTH + 4 * j + 1].texcoord.y = (float)j / HEIGHT;

				data[4 * i*WIDTH + 4 * j + 2].vertex.x += 0.5*i;
				data[4 * i*WIDTH + 4 * j + 2].vertex.y += 0.5*j;
				data[4 * i*WIDTH + 4 * j + 2].texcoord.x = (float)(i+1)/WIDTH;
				data[4 * i*WIDTH + 4 * j + 2].texcoord.y = (float)(j+1)/HEIGHT;

				data[4 * i*WIDTH + 4 * j + 3].vertex.x += 0.5*i;
				data[4 * i*WIDTH + 4 * j + 3].vertex.y += 0.5*j;
				data[4 * i*WIDTH + 4 * j + 3].texcoord.x = (float)i/WIDTH;
				data[4 * i*WIDTH + 4 * j + 3].texcoord.y = (float)(j+1)/HEIGHT;
				sizeof(quad);
				sizeof(struct meshData);
			}
		}
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	shader_init(&quadp, "quad");
	shader_source(&quadp, GL_FRAGMENT_SHADER, quad_frag, quad_frag_len);
	shader_source(&quadp, GL_VERTEX_SHADER, standard_vert, standard_vert_len);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void *)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *)24);

	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++) {
			textureBuffer[WIDTH*i + j] = perlin2d((float)i/WIDTH, (float)j/HEIGHT, 10, 4);
		}
	}
	glGenTextures(1, &to);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, to);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Linear filter might cause a fallback to software rendering so we are using GL_NEAREST_MIPMAP_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//Make sure your GPU support mipmaps with floating point textures

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, &textureBuffer[0]);

	glGenerateMipmap(GL_TEXTURE_2D);
	glUseProgram(quadp.program);
	glUniform1i(glGetUniformLocation(quadp.program, "tex"), 0);
}

void draw_quad()
{
	glBindVertexArray(vao);
	glUseProgram(quadp.program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, to);
	shader_verify(&quadp);
	glDrawArrays(GL_QUADS, 0, WIDTH*HEIGHT*4);
}