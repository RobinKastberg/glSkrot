#include "stdafx.h"
#pragma once

struct fbo {
	GLuint handle;
	GLuint depth_texture;
	GLuint width;
	GLuint height;
	GLuint texture;
};
void fbo_new(struct fbo *self, int width, int height);
void fbo_render_to_texture(struct fbo *self);
void fbo_done(struct fbo *self);