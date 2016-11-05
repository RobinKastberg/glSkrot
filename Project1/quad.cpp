#include "stdafx.h"

#define WIDTH 64
#define HEIGHT 64
#define TEXTURE_SIZE 512
#define WIREFRAME false

struct meshData {
	vec3 vertex;
	vec3 normal;
	vec2 texcoord;
};

static struct meshData data[(WIDTH)*(HEIGHT)];
unsigned int indices[WIDTH*HEIGHT * 4];
static GLuint vbo;
static GLuint vio;
static GLuint vao;
static GLuint ubo;
static GLuint to;
static struct shader_program quadp;

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
			data[i*WIDTH + j].vertex.x = (float)j / WIDTH;
			data[i*WIDTH + j].vertex.y = (float)i / HEIGHT;
			data[i*WIDTH + j].vertex.z = 0.4*perlin2d((float)j / WIDTH, (float)i / HEIGHT, 1, 8);
		}
	}

	for (int i = 0; i < (WIDTH)-1 ; i++)
	{
		for (int j = 0; j < (HEIGHT)-1; j++)
		{
			float tr = data[(i + 1)*WIDTH + j + 1].vertex.z;
			float t = data[(i + 1)*WIDTH + j].vertex.z;
			float tl = data[(i + 1)*WIDTH + j - 1].vertex.z;
			
			float l = data[(i )*WIDTH + j - 1].vertex.z;
			float r = data[(i )*WIDTH + j + 1].vertex.z;

			float bl = data[(i - 1)*WIDTH + j - 1].vertex.z;
			float b = data[(i - 1)*WIDTH + j ].vertex.z;
			float br = data[(i - 1)*WIDTH + j + 1].vertex.z;
			vec3 dx = vec3_new(1.0 / WIDTH, 0.0, data[i*WIDTH + j + 1].vertex.z - data[i*WIDTH + j].vertex.z);
			vec3 dy = vec3_new(0.0, 1.0 / HEIGHT, data[(i+1)*WIDTH + j].vertex.z - data[(i)*WIDTH + j].vertex.z);
			vec3 normal;
			vec3_cross(&dx, &dy, &normal);
			//vec3 normal = vec3_new(tr + 2 * r + br - (tl + 2 * l + bl), (tl + 2 * t + tr) - (bl + 2 * b + br), 1.0/WIDTH);
			vec3_normalize(&normal);
			data[i*WIDTH + j].normal = normal;
		}
	}


	for (int i = 0; i < WIDTH-1; i++)
	{
		for (int j = 0; j < HEIGHT-1; j++)
		{
			indices[4*WIDTH*i + 4*j + 0] = WIDTH* i    + j;
			indices[4*WIDTH*i + 4*j + 1] = WIDTH*(i) + j + 1;
			indices[4*WIDTH*i + 4*j + 2] = WIDTH*(i+1) + j + 1;
			indices[4*WIDTH*i + 4*j + 3] = WIDTH*(i+1)     + j;
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glGenBuffers(1, &vio);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, WIDTH*HEIGHT*4*sizeof(int), indices, GL_STATIC_DRAW);
	

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void *)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *)24);

	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	shader_init(&quadp, "quad");
	shader_source(&quadp, GL_FRAGMENT_SHADER, quad_frag, quad_frag_len);
	shader_source(&quadp, GL_VERTEX_SHADER, standard_vert, standard_vert_len);
	glUseProgram(quadp.program);
	glUniform1i(glGetUniformLocation(quadp.program, "tex"), 1);
}

void draw_quad()
{
	glBindVertexArray(vao);
	glUseProgram(quadp.program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, to);
	shader_verify(&quadp);
	//glDrawArrays(GL_QUADS, 0, WIDTH*HEIGHT * 4);
	if (WIREFRAME) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(5, 5);
		glLineWidth(2.0);
		glDrawElements(GL_QUADS, WIDTH*HEIGHT * 4, GL_UNSIGNED_INT, NULL);
		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glDrawElements(GL_QUADS, WIDTH*HEIGHT * 4, GL_UNSIGNED_INT, NULL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_POLYGON_OFFSET_FILL);
}