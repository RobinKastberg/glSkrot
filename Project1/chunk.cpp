#include "stdafx.h"
#define byte4(x,y,z,w) ((w << 24) | (z << 16) | (y << 8) | x)
chunk::chunk() {
	memset(blk, 0, sizeof(blk));
	elements = 0;
	changed = true;
	glGenBuffers(1, &vbo);
}

chunk::~chunk() {
	glDeleteBuffers(1, &vbo);
}

GLubyte chunk::get(int x, int y, int z) {
	return blk[x][y][z];
}

void chunk::set(int x, int y, int z, GLubyte type) {
	blk[x][y][z] = type;
	changed = true;
}

void chunk::update() {
	changed = false;

	int vertex[CX * CY * CZ * 6 * 6];
	int i = 0;

	for (int x = 0; x < CX; x++) {
		for (int y = 0; y < CY; y++) {
			for (int z = 0; z < CZ; z++) {
				GLubyte type = blk[x][y][z];

				// Empty block?
				if (!type)
					continue;

				// View from negative x
				vertex[i++] = byte4(x, y, z, type);
				vertex[i++] = byte4(x, y, z + 1, type);
				vertex[i++] = byte4(x, y + 1, z, type);
				vertex[i++] = byte4(x, y + 1, z, type);
				vertex[i++] = byte4(x, y, z + 1, type);
				vertex[i++] = byte4(x, y + 1, z + 1, type);

				vertex[i++] = byte4(x + 1, y, z, type);
				vertex[i++] = byte4(x + 1, y + 1, z, type);
				vertex[i++] = byte4(x + 1, y, z + 1, type);
				vertex[i++] = byte4(x + 1, y + 1, z, type);
				vertex[i++] = byte4(x + 1, y + 1, z + 1, type);
				vertex[i++] = byte4(x + 1, y, z + 1, type);

				vertex[i++] = byte4(x, y, z, type);
				vertex[i++] = byte4(x + 1, y, z, type);
				vertex[i++] = byte4(x, y, z + 1, type);
				vertex[i++] = byte4(x + 1, y, z, type);
				vertex[i++] = byte4(x + 1, y, z + 1, type);
				vertex[i++] = byte4(x, y, z + 1, type);

				vertex[i++] = byte4(x, y + 1, z, type);
				vertex[i++] = byte4(x, y + 1, z + 1, type);
				vertex[i++] = byte4(x + 1, y + 1, z, type);
				vertex[i++] = byte4(x + 1, y + 1, z, type);
				vertex[i++] = byte4(x, y + 1, z + 1, type);
				vertex[i++] = byte4(x + 1, y + 1, z + 1, type);

				vertex[i++] = byte4(x, y, z, type);
				vertex[i++] = byte4(x, y + 1, z, type);
				vertex[i++] = byte4(x + 1, y, z, type);
				vertex[i++] = byte4(x, y + 1, z, type);
				vertex[i++] = byte4(x + 1, y + 1, z, type);
				vertex[i++] = byte4(x + 1, y, z, type);

				vertex[i++] = byte4(x, y, z + 1, type);
				vertex[i++] = byte4(x + 1, y, z + 1, type);
				vertex[i++] = byte4(x, y + 1, z + 1, type);
				vertex[i++] = byte4(x, y + 1, z + 1, type);
				vertex[i++] = byte4(x + 1, y, z + 1, type);
				vertex[i++] = byte4(x + 1, y + 1, z + 1, type);


			}
		}
	}

	elements = i;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertex, vertex, GL_STATIC_DRAW);
}

void chunk::render() {
	if (changed)
		update();

	// If this chunk is empty, we don't need to draw anything.
	if (!elements)
		return;

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, elements);
}