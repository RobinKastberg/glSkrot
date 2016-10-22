#include "stdafx.h"
#pragma warning(disable:4554)
#define byte4(x,y,z,w) ((w << 24) | (z << 16) | (y << 8) | x)

chunk::chunk() {
	memset(blk, 0, sizeof(blk));
	elements = 0;
	changed = true;
	glGenBuffers(2, vbos);
}

chunk::~chunk() {
	glDeleteBuffers(2, vbos);
}

GLubyte chunk::get(int x, int y, int z) const {
	return blk[x][y][z];
}

void chunk::set(int x, int y, int z, GLubyte type) {
	blk[x][y][z] = type;
	changed = true;
}

void chunk::update() {
	changed = false;

	int vertex[CX * CY * CZ * 6 * 6];
	int normals[CX * CY * CZ * 6];
	int i = 0;
	int j = 0;

	for (int x = 0; x < CX; x++) {
		for (int y = 0; y < CY; y++) {
			for (int z = 0; z < CZ; z++) {
				GLubyte type = blk[x][y][z];

				// Empty block?
				if (!type)
					continue;
				GLubyte up = blk[x][y][z+1];
				GLubyte down = blk[x][y][z-1];
				GLubyte xplus = blk[x+1][y][z];
				GLubyte xminus = blk[x-1][y][z];
				GLubyte yplus = blk[x][y+1][z];
				GLubyte yminus = blk[x][y-1][z];
				// View from negative x
				/* TODO INVERT CONDITIONS */
				if (1 || x == 0 || !xminus) {
					vertex[i++] = byte4(x, y, z, type);
					normals[j++] = byte4(-1, 0, 0, 0);
					vertex[i++] = byte4(x, y, z + 1, type);
					normals[j++] = byte4(-1, 0, 0, 0);
					vertex[i++] = byte4(x, y + 1, z, type);
					normals[j++] = byte4(-1, 0, 0, 0);
					vertex[i++] = byte4(x, y + 1, z, type);
					normals[j++] = byte4(-1, 0, 0, 0);
					vertex[i++] = byte4(x, y, z + 1, type);
					normals[j++] = byte4(-1, 0, 0, 0);
					vertex[i++] = byte4(x, y + 1, z + 1, type);
					normals[j++] = byte4(-1, 0, 0, 0);
				}
				if (1 || !xplus) {
					vertex[i++] = byte4(x + 1, y, z, type);
					normals[j++] = byte4(1, 0, 0, 0);
					vertex[i++] = byte4(x + 1, y + 1, z, type);
					normals[j++] = byte4(1, 0, 0, 0);
					vertex[i++] = byte4(x + 1, y, z + 1, type);
					normals[j++] = byte4(1, 0, 0, 0);
					vertex[i++] = byte4(x + 1, y + 1, z, type);
					normals[j++] = byte4(1, 0, 0, 0);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, type);
					normals[j++] = byte4(1, 0, 0, 0);
					vertex[i++] = byte4(x + 1, y, z + 1, type);
					normals[j++] = byte4(1, 0, 0, 0);
				}
				if (1 || y==0 ||!yminus) {
					vertex[i++] = byte4(x, y, z, type);
					normals[j++] = byte4(0, -1, 0, 0);
					vertex[i++] = byte4(x + 1, y, z, type);
					normals[j++] = byte4(0, -1, 0, 0);
					vertex[i++] = byte4(x, y, z + 1, type);
					normals[j++] = byte4(0, -1, 0, 0);
					vertex[i++] = byte4(x + 1, y, z, type);
					normals[j++] = byte4(0, -1, 0, 0);
					vertex[i++] = byte4(x + 1, y, z + 1, type);
					normals[j++] = byte4(0, -1, 0, 0);
					vertex[i++] = byte4(x, y, z + 1, type);
					normals[j++] = byte4(0, -1, 0, 0);
				}
				if (1 || !yplus) {
					vertex[i++] = byte4(x, y + 1, z, type);
					normals[j++] = byte4(0, 1, 0, 0);
					vertex[i++] = byte4(x, y + 1, z + 1, type);
					normals[j++] = byte4(0, 1, 0, 0);
					vertex[i++] = byte4(x + 1, y + 1, z, type);
					normals[j++] = byte4(0, 1, 0, 0);
					vertex[i++] = byte4(x + 1, y + 1, z, type);
					normals[j++] = byte4(0, 1, 0, 0);
					vertex[i++] = byte4(x, y + 1, z + 1, type);
					normals[j++] = byte4(0, 1, 0, 0);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, type);
					normals[j++] = byte4(0, 1, 0, 0);
				}
				if (1 ||z == 0 || down) {
					vertex[i++] = byte4(x, y, z, type);
					normals[j++] = byte4(0, 0, -1, 0);
					vertex[i++] = byte4(x, y + 1, z, type);
					normals[j++] = byte4(0, 0, -1, 0);
					vertex[i++] = byte4(x + 1, y, z, type);
					normals[j++] = byte4(0, 0, -1, 0);
					vertex[i++] = byte4(x, y + 1, z, type);
					normals[j++] = byte4(0, 0, -1, 0);
					vertex[i++] = byte4(x + 1, y + 1, z, type);
					normals[j++] = byte4(0, 0, -1, 0);
					vertex[i++] = byte4(x + 1, y, z, type);
					normals[j++] = byte4(0, 0, -1, 0);
				}
				if (1 ||!up) {
					vertex[i++] = byte4(x, y, z + 1, type);
					normals[j++] = byte4(0, 0, 1, 0);
					vertex[i++] = byte4(x + 1, y, z + 1, type);
					normals[j++] = byte4(0, 0, 1, 0);
					vertex[i++] = byte4(x, y + 1, z + 1, type);
					normals[j++] = byte4(0, 0, 1, 0);
					vertex[i++] = byte4(x, y + 1, z + 1, type);
					normals[j++] = byte4(0, 0, 1, 0);
					vertex[i++] = byte4(x + 1, y, z + 1, type);
					normals[j++] = byte4(0, 0, 1, 0);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, type);
					normals[j++] = byte4(0, 0, 1, 0);
				}
			}
		}
	}

	elements = i;
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, elements * 4, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, j * 4, normals, GL_STATIC_DRAW);
}

void chunk::render() {
	if (changed)
		update();

	// If this chunk is empty, we don't need to draw anything.
	if (!elements)
		return;
	
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glVertexAttribPointer(1, 4, GL_BYTE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0, elements);
}