#pragma once
#include "stdafx.h"
#define CX 16
#define CY 16
#define CZ 16
struct chunk {
	GLubyte blk[CX][CY][CZ];
	GLuint vbo;
	int elements;
	bool changed;

	chunk();

	~chunk();

	GLubyte get(int x, int y, int z);

	void set(int x, int y, int z, GLubyte type);

	void update();

	void render();
};
