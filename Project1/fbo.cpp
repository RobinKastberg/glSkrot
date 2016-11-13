#include "stdafx.h"

void fbo_new(struct fbo *self, int width, int height)
{
	self->width = width;
	self->height = height;
	glGenFramebuffers(1, &self->handle);
	glGenTextures(1, &self->depth_texture);
	glGenTextures(1, &self->texture);
	texture(&self->texture, BUFFER_COLOR, self->width, self->height, NULL);
	texture(&self->depth_texture, BUFFER_DEPTH, self->width, self->height, NULL);

}
void fbo_render_to_texture(struct fbo *self)
{
	int w, h;
	glBindFramebuffer(GL_FRAMEBUFFER, self->handle);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, self->depth_texture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, self->width, self->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void fbo_done(struct fbo *self)
{
	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}