#include "stdafx.h"
#define HEIGHTMAP_SIZE 256
#define NORMALMAP_SIZE 256

struct 
{
	GLuint fbo;
	GLuint height_map;
	GLuint normal_map;
} loader;

void texture(GLuint *handle, GLenum format, int width, int height,GLenum *params)
{
	glGenTextures(1, handle);
	glBindTexture(GL_TEXTURE_2D, *handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	for (;params != NULL && *params != 0; params += 2)
	{
		glTexParameteri(GL_TEXTURE_2D, (*params), (*params+2));
	}
	switch (format)
	{
		case BUFFER_COLOR:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		break;
		case BUFFER_FLOAT:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			break;
		case BUFFER_DEPTH:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
			break;
		case BUFFER_MONO:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
			break;
	}
	
}

void loader_load()
{
	glGenFramebuffers(1, &loader.fbo);
	glGenTextures(2, &loader.height_map);

	
	texture(&loader.normal_map, BUFFER_MONO,  HEIGHTMAP_SIZE, HEIGHTMAP_SIZE, 0);
	texture(&loader.height_map, BUFFER_COLOR, NORMALMAP_SIZE, NORMALMAP_SIZE, 0);
	
}