#include "stdafx.h"
#define HEIGHTMAP_SIZE 256
#define NORMALMAP_SIZE 256

struct 
{
	GLuint fbo;
	GLuint height_map;
	GLuint normal_map;
} loader;
void draw_quad();

void pprocess_new(struct pprocess *self, int width, int height, struct shader_program *sp)
{
	glGenFramebuffers(1, &self->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, self->fbo);
	glGenTextures(1, &self->out_texture);
	texture(&self->out_texture, BUFFER_COLOR, width, height, NULL);
	self->sp = sp;
	self->width = width;
	self->height = height;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->out_texture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_NONE);
	//glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}
void pprocess_size(struct pprocess *self, int width, int height)
{
	self->width = width;
	self->height = height;
	texture(&self->out_texture, GL_RGBA8, width, height, NULL);
}
GLuint pprocess_do(struct pprocess *self, GLuint in_texture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, self->fbo);
	{
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, in_texture);
		glUseProgram(self->sp->program);
		glUniform1i(glGetUniformLocation(self->sp->program, "pprocess_in"), 10);
		glViewport(0, 0, self->width, self->height);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		draw_quad();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return self->out_texture;
}
void texture(GLuint *handle, GLenum format, int width, int height,GLenum *params)
{
	//glGenTextures(1, handle);
	glBindTexture(GL_TEXTURE_2D, *handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	for (;params != NULL && *params != 0; params += 2)
	{
		glTexParameteri(GL_TEXTURE_2D, (*params), *(params+1));
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