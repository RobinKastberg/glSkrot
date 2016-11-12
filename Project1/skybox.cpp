#include "stdafx.h"

#define WIDTH 100
#define HEIGHT 100
#define TEXTURE_SIZE 512
#define NOISE(x,y,z) perlin3d(x,y,z, 3, 4)
static struct meshData data[WIDTH*HEIGHT * 4];

static GLuint vbo;
static GLuint vao;
static GLuint ubo;
static GLuint to;
static struct shader_program skyboxp;

static float textureUp[2 * 512*512];
static float textureDown[2 * 512 * 512];
static float textureLeft[2 * 512 * 512];
static float textureRight[2 * 512 * 512];
static float textureFront[2 * 512 * 512];
static float textureBack[2 * 512 * 512];

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

void init_skybox()
{

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	shader_init(&skyboxp, "skybox");
	shader_source(&skyboxp, GL_FRAGMENT_SHADER, skybox_frag, skybox_frag_len);
	shader_source(&skyboxp, GL_VERTEX_SHADER, skybox_vert, skybox_vert_len);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void *)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *)24);

	for (int i = 0; i < TEXTURE_SIZE; i++)
	{
		for (int j = 0; j < TEXTURE_SIZE; j++) {
			float z = 2*(((float)j) / TEXTURE_SIZE - 0.5);
			float x = 2 * ((TEXTURE_SIZE - (float)i) / TEXTURE_SIZE - 0.5);
			float y = 1;
			vec3 test = vec3_new(x, y, z);
			vec3_normalize(&test);
			float test2 = perlin3d(1 + test.x, 1 + test.y, 1 + test.z, 10, 4);

			textureFront[2 * TEXTURE_SIZE*i + 2 * j] = NOISE(test.x, test.y, test.z);
			textureFront[2*TEXTURE_SIZE*i + 2*j + 1] = 0.0;
				//perlin2d((float)i / (TEXTURE_SIZE), (float)j / (TEXTURE_SIZE), 10, 4);
		}
	}

	for (int i = 0; i < TEXTURE_SIZE; i++)
	{
		for (int j = 0; j < TEXTURE_SIZE; j++) {
			float z = 2 * (((float)i) / TEXTURE_SIZE - 0.5);
			float y = 2 * ((TEXTURE_SIZE - (float)j) / TEXTURE_SIZE - 0.5);
			float x = 1;
			vec3 test = vec3_new(x, y, z);
			vec3_normalize(&test);
			textureRight[2 * TEXTURE_SIZE*i + 2 * j] = NOISE(test.x, test.y, test.z);
			textureRight[2 * TEXTURE_SIZE*i + 2 * j + 1] = 0;
			//perlin2d((float)i / (TEXTURE_SIZE), (float)j / (TEXTURE_SIZE), 10, 4);
		}
	}

	for (int i = 0; i < TEXTURE_SIZE; i++)
	{
		for (int j = 0; j < TEXTURE_SIZE; j++) {
			float z = 2 * ((TEXTURE_SIZE - (float)j) / TEXTURE_SIZE - 0.5);
			float x = 2 * ((TEXTURE_SIZE - (float)i) / TEXTURE_SIZE - 0.5);
			float y = -1;
			vec3 test = vec3_new(x, y, z);
			vec3_normalize(&test);
			textureBack[2 * TEXTURE_SIZE*i + 2 * j] = NOISE(test.x, test.y, test.z);
			textureBack[2 * TEXTURE_SIZE*i + 2 * j + 1] = 0.0;
			//perlin2d((float)i / (TEXTURE_SIZE), (float)j / (TEXTURE_SIZE), 10, 4);
		}
	}

	for (int i = 0; i < TEXTURE_SIZE; i++)
	{
		for (int j = 0; j < TEXTURE_SIZE; j++) {
			float y = 2 * ((TEXTURE_SIZE-(float)j) / TEXTURE_SIZE - 0.5);
			float x = -1;
			float z = 2 * ((TEXTURE_SIZE - (float)i) / TEXTURE_SIZE - 0.5);
			vec3 test = vec3_new(x, y, z);
			vec3_normalize(&test);
			textureLeft[2 * TEXTURE_SIZE*i + 2 * j] = NOISE(test.x, test.y, test.z);
			textureLeft[2 * TEXTURE_SIZE*i + 2 * j + 1] = 0.0;
			//perlin2d((float)i / (TEXTURE_SIZE), (float)j / (TEXTURE_SIZE), 10, 4);
		}
	}

	for (int i = 0; i < TEXTURE_SIZE; i++)
	{
		for (int j = 0; j < TEXTURE_SIZE; j++) {
			float y = 2 * ((TEXTURE_SIZE-(float)j) / TEXTURE_SIZE - 0.5);
			float x = 2 * ((TEXTURE_SIZE- (float)i) / TEXTURE_SIZE - 0.5);
			float z = 1;
			vec3 test = vec3_new(x, y, z);
			vec3_normalize(&test);
			textureUp[2 * TEXTURE_SIZE*i + 2 * j] = NOISE(test.x, test.y, test.z);
			textureUp[2 * TEXTURE_SIZE*i + 2 * j + 1] = 0.0;
			//perlin2d((float)i / (TEXTURE_SIZE), (float)j / (TEXTURE_SIZE), 10, 4);
		}
	}

	for (int i = 0; i < TEXTURE_SIZE; i++)
	{
		for (int j = 0; j < TEXTURE_SIZE; j++) {
			float y = 2 * (((float)j) / TEXTURE_SIZE - 0.5);
			float x = 2 * ((TEXTURE_SIZE - (float)i) / TEXTURE_SIZE - 0.5);
			float z = -1;
			vec3 test = vec3_new(x, y, z);
			vec3_normalize(&test);
			textureDown[2 * TEXTURE_SIZE*i + 2 * j] = NOISE(test.x, test.y, test.z);
			textureDown[2 * TEXTURE_SIZE*i + 2 * j + 1] = 0.0;
			//perlin2d((float)i / (TEXTURE_SIZE), (float)j / (TEXTURE_SIZE), 10, 4);
		}
	}
	GLuint noise;
	glGenTextures(1, &noise);
	glGenTextures(1, &to);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, to);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Linear filter might cause a fallback to software rendering so we are using GL_NEAREST_MIPMAP_NEAREST
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//Make sure your GPU support mipmaps with floating point textures
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RG, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, &textureBack[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RG, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, &textureRight[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RG, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, &textureFront[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RG, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, &textureLeft[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RG, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, &textureUp[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RG, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, &textureDown[0]);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noise);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RG, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RG, GL_FLOAT, &textureFront[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Linear filter might cause a fallback to software rendering so we are using GL_NEAREST_MIPMAP_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);

	shader_use(&skyboxp);
	glUniform1i(glGetUniformLocation(skyboxp.program, "tex"), 0);
	glUniform1i(glGetUniformLocation(skyboxp.program, "tex2d"), 1);

}

void draw_skybox()
{
	glBindVertexArray(vao);
	shader_use(&skyboxp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, to);
	shader_verify(&skyboxp);
	glDrawArrays(GL_QUADS, 0, 4);
}