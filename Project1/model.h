#pragma once

#include <vector>

struct edge {
	struct vertex *v0;
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
};
struct face {
	struct edge *edge;
	float normal[3];
};
struct model {
	std::vector<face*>   faces;
	std::vector<vertex*> verts;
	std::vector<edge*>   edges;
	

	GLuint vbo;
};
struct edge *edge_new(face *f, vertex *v0, vertex *v1);
void find_twins(struct edge **edges, int len);
void draw(struct model *m);
struct model *make_cube();