#include "stdafx.h"
#include "model.h"
#include <vector>
#include <algorithm>
#pragma warning(disable:4554)
#define byte4(x,y,z,w) ((w << 24) | (z << 16) | (y << 8) | x)

chunk::chunk() {
	memset(blk, 0, sizeof(blk));
	elements = 0;
	changed = true;
	m = new model();
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
#define VERTICES CX * CY * CZ * 8
#define INDICES  CX * CY * CZ * 12 * 3
int find_or_add(int *vertices, int compare, int *index)
{
	
	for (int i = 0; i < VERTICES; i++)
	{
		if (vertices[i] == compare)
			return i;
	}
	int i = *index;
	vertices[*index] = compare;
	(*index)++;
	return i;
}
void chunk::update() {
	changed = false;

	int vertices[VERTICES];
	int indices[INDICES];
	int v_i = 0;
	int i_i = 0;

	for (int x = 0; x < CX; x++) {
		for (int y = 0; y < CY; y++) {
			for (int z = 0; z < CZ; z++) {
				GLubyte type = blk[x][y][z];

				// Empty block?
				if (!type)
					continue;
				
				// View from negative x
				if (x == 0 || blk[x - 1][y][z] != type) {
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z, type), &v_i);
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z + 1, type), &v_i);
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z, type), &v_i);
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z, type), &v_i);
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z + 1, type), &v_i);
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z + 1, type), &v_i);
				}
				if (x == 15 || blk[x + 1][y][z] != type) {
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z + 1, type), &v_i);
					
				}
				if (y==0 || blk[x][y-1][z] != type) {
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z + 1, type), &v_i);
					
				}
				if (y==15 || blk[x][y+1][z] != type) {
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z + 1, type), &v_i);
					
				}
				if (z == 0 || blk[x][y][z-1] != type) {
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z, type), &v_i);
					
				}
				if (z == 15 || blk[x][y][z+1] != type) {
					indices[i_i++] = find_or_add(vertices, byte4(x, y, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x, y + 1, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y, z + 1, type), &v_i);
					
					indices[i_i++] = find_or_add(vertices, byte4(x + 1, y + 1, z + 1, type), &v_i);
					
				}
				
			}
		}
	}
	elements = v_i;


	if (v_i) {
		// DELETE M IF ALREADY THERE.
		this->m = make_model(vertices, indices, v_i, i_i);
	}
}

void chunk::render() {
	if (changed)
		update();

	// If this chunk is empty, we don't need to draw anything.
	if (!elements)
		return;
	
	draw(m);
}