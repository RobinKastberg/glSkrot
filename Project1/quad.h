#pragma once
#include "stdafx.h"
struct quad {
	struct mesh mesh;
};
void quad_new(struct quad *self);
void quad_draw(struct quad *self);
void draw_quad(struct shader_program *sp);