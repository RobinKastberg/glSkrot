#pragma once

#include <vector>

struct edge {
	struct vertex *v0;
	short v0i;
	short v1i;
	struct vertex *v1;
	struct edge *pair;
	struct face *face;
	struct edge *next;
	float newCoord[3];
	int mark = 0;
};
struct vertex {
	struct edge *out; 
	float coord[3];
	float newCoord[3];
	int mark = 0;

	vertex(float x, float y, float z) {
		coord[0] = x;
		coord[1] = y;
		coord[2] = z;
	}
	vertex() {}
};
struct face {
	struct edge *edge;
	float normal[3];
};
struct model {
	std::vector<face*>   faces;
	std::vector<vertex*> verts;
	std::vector<edge*>   edges;
	
	int up_to_date = 0;
	int face_count;
	GLuint vbo;
	GLuint64 vbo_addr = 0;
	int vbo_size = 0;
};

struct _hash_entry {
	int key;
	short value;
};

struct _hash {
	struct _hash_entry *mem;
	int size;
};
struct edge *edge_new(model *m, struct face *f, int v0i, int v1i);
void find_twins(std::vector<edge *> edges);
void draw(struct model *m);
struct model *make_cube();
void make_model(struct model *m, int *cube_vertices, short *indices, int vsize, int isize);