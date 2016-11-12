#pragma once
#include "stdafx.h"
struct terrain {
	struct mesh mesh;
	unsigned int width;
	unsigned int height;
	unsigned int max_lod;
	float lod;
};

//void quad_new(struct quad *self, unsigned int lod);
void terrain_new(struct terrain *self, unsigned int max_lod, float xoff = 0.0f, float yoff = 0.0f);

void terrain_draw(struct terrain *self);

void init_skybox();
void draw_skybox();