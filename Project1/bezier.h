#pragma once
#include "stdafx.h"
struct curve {
	struct mesh mesh;
	vec3 p[3];
	int steps;
};
struct surface {
	struct mesh mesh;
	vec3 p[9];
	int steps;
	mat4 localMatrix;
};
void curve_new(struct curve *self, unsigned int steps);
void curve_draw(struct curve *self);
void curve_prepare(struct curve *self);
vec3 curve_get_position(struct curve *self, float u);

void surface_new(struct surface *self, unsigned int steps);
void surface_draw(struct surface *self);
void surface_prepare(struct surface *self);
vec3 surface_get_position(struct surface *self, float u, float v);