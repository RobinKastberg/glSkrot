#pragma once
#include "stdafx.h"
#include "model.h"
#define CX 64
#define CY 64
#define CZ 64
#define SCX 8
#define SCY 8
#define SCZ 8


struct chunk {
	GLubyte blk[CX][CY][CZ];
	GLuint vbos[2];
	GLuint vao;
	int elements;
	bool changed;
	model *m;
	struct _hash vertex_cache;

	chunk();

	~chunk();

	GLubyte get(int x, int y, int z) const;

	void set(int x, int y, int z, GLubyte type);

	int find_or_add(int * vertices, int compare, int * index);

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