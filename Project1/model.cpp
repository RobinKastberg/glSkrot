#include "stdafx.h"

#include "model.h"

#include <memory.h>
float cube_vertices[] = {
	// front
	0,0,1,
	1,0,1,
	1,1,1,
	0,1,1,
	// back
	0,0,0,
	1,0,0,
	1,1,0,
	0,1,0,
};
int cube_elements[] = {
	// front
	0, 1, 2,
	2, 3, 0,
	// top
	1, 5, 6,
	6, 2, 1,
	// back
	7, 6, 5,
	5, 4, 7,
	// bottom
	4, 0, 3,
	3, 7, 4,
	// left
	4, 5, 1,
	1, 0, 4,
	// right
	3, 2, 6,
	6, 7, 3,
};
struct edge *edge_new(struct face *f, struct vertex *v0, struct vertex *v1)
{
	struct edge *e = new edge();
	v0->out = e;
	e->v0 = v0;
	e->v1 = v1;
	e->face = f;
	return e;
}
void find_twins(struct edge *edges[], int len)
{
	for (int i = 0; i < len; i++)
		if (edges[i]->pair == NULL)
			for (int j = 0; j < len; j++)
				if (edges[i]->v0 == edges[j]->v1
				 && edges[i]->v1 == edges[j]->v0)
					edges[i]->pair = edges[j];
}
void draw(struct model *m)
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < m->faceCount; i++)
	{
		glVertexAttrib3fv(0,m->faces[i]->edge->v0->coord);
		glVertexAttrib3fv(0,m->faces[i]->edge->v1->coord);
		glVertexAttrib3fv(0,m->faces[i]->edge->next->v0->coord);
	}
	glEnd();
}
struct model *make_cube()
{
	vertex *v = new vertex[8];
	edge **e = new edge*[36];
	face **faces = new face*[12];
	model *m = new model();
	for (int i = 0; i < 8; i++)
		memcpy(v[i].coord, &cube_vertices[3*i], sizeof(float) * 3);
	for (int i = 0; i < 12;i++) {
		face *f = new face();
		edge *e1 = new edge();
		edge *e2 = new edge();
		edge *e3 = new edge();
		e[3*i] = edge_new(f, v+cube_elements[3*i], v + cube_elements[3 * i + 1]);
		e[3*i+1] = edge_new(f, v + cube_elements[3 * i + 1], v + cube_elements[3 * i + 2]);
		e[3*i+2] = edge_new(f, v + cube_elements[3 * i + 2], v + cube_elements[3 * i]);

		e[3*i]->face = f;
		e[3*i + 1]->face = f;
		e[3*i+2]->face = f;

		e[3*i]->next     = e[3*i + 1];
		e[3*i + 1]->next = e[3*i + 2];
		e[3*i + 2]->next = e[3*i];

		f->edge = e[3*i];

		faces[i] = f;
	}
	find_twins(e, 36);
	m->edges = e;
	m->edgeCount = 36;
	m->faces = faces;
	m->faceCount = 12;
	m->verts = &v;
	m->vertCount = 8;

	return m;
}