#pragma once
#include "stdafx.h"
#define CX 16
#define CY 16
#define CZ 16
#define SCX 16
#define SCY 16
#define SCZ 16
struct chunk {
	GLubyte blk[CX][CY][CZ];
	GLuint vbos[2];
	GLuint vao;
	int elements;
	bool changed;

	chunk();

	~chunk();

	GLubyte get(int x, int y, int z) const;

	void set(int x, int y, int z, GLubyte type);

	void update();

	void render();
};

struct superchunk {
	chunk *c[SCX][SCY][SCZ];

	superchunk();

	~superchunk();

	GLubyte get(int x, int y, int z) const;
	void set(int x, int y, int z, GLubyte type);

	void render();
	void update();
};