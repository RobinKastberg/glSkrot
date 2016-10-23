#pragma once
struct edge {
	struct vertex *v0;
	struct vertex *v1;
	struct edge *pair;
	struct face *face;
	struct edge *next;
};
struct vertex {
	struct edge *out; 
	float coord[3];
};
struct face {
	struct edge *edge;
	float normal[3];
};
struct model {
	struct face **faces;
	size_t faceCount;
	struct vertex **verts;
	size_t vertCount;
	struct edge **edges;
	size_t edgeCount;
};
struct edge *edge_new(face *f, vertex *v0, vertex *v1);
void find_twins(struct edge **edges, int len);
void draw(struct model *m);
struct model *make_cube();