#include "stdafx.h"

#include "model.h"

#include <memory.h>
#include <algorithm>

GLfloat cube_vertices[] = {
	// front
	-1.0, -1.0,  1.0,
	1.0, -1.0,  1.0,
	1.0,  1.0,  1.0,
	-1.0,  1.0,  1.0,
	// back
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0,
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
void chkdsk(struct model *m)
{
	for (int i = 0; i < m->edges.size(); i++)
	{
		edge *e = m->edges[i];
		assert(e->pair != NULL);
		assert(e->next->next->next == e);
		assert(e->pair->pair == e);
		assert(e->face != e->pair->face);
		assert(e->next != e);
		assert(e->next->next != e);
		assert(e->face == e->next->face);
	}
	for (int i = 0; i < m->verts.size(); i++)
	{
		vertex *v = m->verts[i];
		assert(v->out->next->next->next->v0 == v);
		assert(v->out->next->next->v1 == v);
	}
	for (int i = 0; i < m->faces.size(); i++)
	{
		face *f = m->faces[i];
		assert(f->edge->face == f);
		assert(f->edge->next->face == f);
		assert(f->edge->next->next->face == f);
	}
}
void find_twins(std::vector<edge *> edges)
{
	for (int i = 0; i < edges.size(); i++)
		if (edges[i]->pair == NULL)
			for (int j = 0; j < edges.size(); j++)
				if (edges[i]->v0 == edges[j]->v1
				 && edges[i]->v1 == edges[j]->v0)
					edges[i]->pair = edges[j];
}

void draw(struct model *m)
{
	float *glData = new float[2 * 18* m->faces.size()];
	for (int i = 0; i < m->faces.size(); i++)
	{
		face *f = m->faces[i];
		float normal[3];
		normal[0] = f->edge->v0->coord[1]* f->edge->v1->coord[2] - f->edge->v0->coord[2] * f->edge->v1->coord[1];
		normal[1] = f->edge->v0->coord[3] * f->edge->v1->coord[0] - f->edge->v0->coord[0] * f->edge->v1->coord[2];
		normal[2] = f->edge->v0->coord[0] * f->edge->v1->coord[1] - f->edge->v0->coord[1] * f->edge->v1->coord[0];

		memcpy(glData + 36 * i, m->faces[i]->edge->v0->coord, sizeof(float) * 3);
		memcpy(glData + 36 * i + 3, normal, sizeof(float) * 3);
		memcpy(glData + 36 * i + 6, m->faces[i]->edge->v1->coord, sizeof(float) * 3);
		memcpy(glData + 36 * i + 9, normal, sizeof(float) * 3);
		memcpy(glData + 36 * i + 12, m->faces[i]->edge->next->v0->coord, sizeof(float) * 3);
		memcpy(glData + 36 * i + 15, normal, sizeof(float) * 3);
		memcpy(glData + 36 * i + 18, m->faces[i]->edge->next->v1->coord, sizeof(float) * 3);
		memcpy(glData + 36 * i + 21, normal, sizeof(float) * 3);
		memcpy(glData + 36 * i + 24, m->faces[i]->edge->next->next->v0->coord, sizeof(float) * 3);
		memcpy(glData + 36 * i + 27, normal, sizeof(float) * 3);
		memcpy(glData + 36 * i + 30, m->faces[i]->edge->next->next->v1->coord, sizeof(float) * 3);
		memcpy(glData + 36 * i + 33, normal, sizeof(float) * 3);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	glBufferData(GL_ARRAY_BUFFER, 2 * 6 * 3 * m->faces.size() * sizeof(float), glData, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void *)12);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glLineWidth(5);
	glDrawArrays(GL_LINES, 0, 6 *  m->faces.size());
	delete glData;
}
void subdivide(struct model *m)
{
	int len = m->edges.size();

	for (int i = 0; i < m->verts.size(); i++)
	{
		vertex *v = m->verts[i];
		std::vector<vertex *> adjacent;

		for (int j = 0; j<m->edges.size(); j++)
		{
			if (m->edges[j]->v1 == v)
				adjacent.push_back(m->edges[j]->v0);
		}
		int n = adjacent.size();
		float beta = n > 3 ? 3.0 / (8.0*n) : 3.0 / 16.0;
		//float beta = 3.0 / 8.0 + (3.0 / 8.0 + 0.25*cos(2 * 3.1415926535 / n))*(3.0 / 8.0 + 0.25*cos(2 * 3.1415926535 / n));
		for (int k = 0; k < 3; k++)
		{
			v->newCoord[k] = (1.0 - beta*n) * v->coord[k];
		}

		for (int j = 0; j < adjacent.size(); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				v->newCoord[k] += beta * adjacent[j]->coord[k];
				//v->newCoord[k] = v->coord[k];
			}
		}
	}
	for (int i = 0; i < m->edges.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m->edges[i]->newCoord[j] = 0.375*m->edges[i]->v0->coord[j] + 0.375*m->edges[i]->v1->coord[j]
				+ 0.125*m->edges[i]->next->v1->coord[j] + 0.125*m->edges[i]->pair->next->v1->coord[j];
			//m->edges[i]->newCoord[j] = 0.5*m->edges[i]->v0->newCoord[j] + 0.5*m->edges[i]->v1->newCoord[j];
		}
	}
//	std::copy(m->edges.begin(), m->edges.end(), oldEdges.begin());
	for (int i = 0; i <len; i++)
	{
		if (m->edges[i]->mark || m->edges[i]->pair->mark)
			continue;
		edge *e1 = m->edges[i];
		edge *e1new = new edge();
		edge *e1newpair = new edge();
		edge *e2 = e1->pair;
		e1->mark = 1;
		e2->mark = 1;
		vertex *v0 = e1->v0;
		vertex *v1 = e1->v1;
		vertex *new1 = new vertex();
		
		for (int j = 0; j < 3; j++)
		{
			new1->newCoord[j] = e1->newCoord[j];
		}
		//new1->coord[0] = (e1->v0->coord[0] + e1->v1->coord[0]) / 2.0;
		//new1->coord[1] = (e1->v0->coord[1] + e1->v1->coord[1]) / 2.0;
		//new1->coord[2] = (e1->v0->coord[2] + e1->v1->coord[2]) / 2.0;
		new1->out = e1new;
		new1->mark = 1;

		e1new->v0 = new1;
		e1new->v1 = v1;
		e1new->next = e1->next;
		e1new->pair = e2;
		e1new->face = e1->face;

		e1->v1 = new1;
		e1->next = e1new;
		e1->pair = e1newpair;

		e1newpair->v0 = new1;
		e1newpair->v1 = v0;
		e1newpair->next = e2->next;
		e1newpair->pair = e1;
		e1newpair->face = e2->face;
		// FIND PREVIOUS


		e2->v1 = new1;
		e2->next = e1newpair;
		e2->pair = e1new;


		assert(e1->pair->pair == e1);
		assert(e1new->pair->pair == e1new);
		assert(e1newpair->pair->pair == e1newpair);
		assert(e2->pair->pair == e2);
		assert(e1->next->v1 == v1);
		assert(e1->next->pair->next->v1 == v0);
		assert(e1new->pair->next->v1 == v0);
		assert(e1new->pair->next != NULL);
		assert(e1->next->next != NULL);
		assert(e2->next != NULL);
		assert(e2->next->next != NULL);
		assert(e1new->next != NULL);
		assert(e1new->next->next != NULL);
		assert(e1newpair->next != NULL);
		assert(e1newpair->next->next != NULL);
		m->edges.push_back(e1new);
		m->edges.push_back(e1newpair);
		m->verts.push_back(new1);
	}
	len = m->faces.size();
	for (int i = 0; i < len; i++)
	{
		face *f = m->faces[i];
		face *newf1 = new face();
		face *newf2 = new face();
		face *newf3 = new face();
		edge *e1 = f->edge;
		edge *e2 = e1->next;
		edge *e3 = e2->next;
		edge *e4 = e3->next;
		edge *e5 = e4->next;
		edge *e6 = e5->next;
		edge *new1 = new edge();
		edge *new2 = new edge();
		edge *new3 = new edge();

		e1->next = new edge();
		e1->next->v0 = e1->v1;
		e1->next->v1 = e6->v0;
		e1->next->pair = new1;
		e1->next->next = e6;

		e1->next->face = f;
		e6->face = f; //reuse current face

		e3->next = new edge();
		e3->next->v0 = e3->v1;
		e3->next->v1 = e2->v0;
		e3->next->pair = new2;
		e3->next->next = e2;

		newf1->edge = e3;
		
		e2->face = newf1;
		e3->face = newf1;
		e3->next->face = newf1;

		e5->next = new edge();
		e5->next->v0 = e5->v1;
		e5->next->v1 = e4->v0;
		e5->next->pair = new3;
		e5->next->next = e4;

		e5->next->face = newf2;
		newf2->edge = e5->next;
		e4->face = newf2;
		e5->face = newf2;


		newf3->edge = new1;

		new1->face = newf3;
		new1->next = new2;
		new1->pair = e1->next;
		new1->v0 = e6->v0;
		new1->v1 = e1->v1;

		new2->face = newf3;
		new2->next = new3;
		new2->pair = e3->next;
		new2->v0 = e2->v0;
		new2->v1 = e3->v1;

		new3->face = newf3;
		new3->next = new1;
		new3->pair = e5->next;
		new3->v0 = e4->v0;
		new3->v1 = e5->v1;

		assert(f->edge->next->next->next == f->edge);

		assert(newf1->edge->next->next->next == newf1->edge);
		assert(newf2->edge->next->next->next == newf2->edge);
		assert(newf3->edge->next->next->next == newf3->edge);
		assert(new1->pair->pair == new1);
		assert(new2->pair->pair == new2);
		assert(new3->pair->pair == new3);
		assert(e1->next->pair->pair == e1->next);
		assert(e3->next->pair->pair == e3->next);
		assert(e5->next->pair->pair == e5->next);


		m->edges.push_back(e1->next);
		m->edges.push_back(e3->next);
		m->edges.push_back(e5->next);

		m->edges.push_back(new1);
		m->edges.push_back(new2);
		m->edges.push_back(new3);

		m->faces.push_back(newf1);
		m->faces.push_back(newf2);
		m->faces.push_back(newf3);
	}
	chkdsk(m);
	len = m->verts.size();
	
	
	for (int i = 0; i < len; i++)
	{
		vertex *v = m->verts[i];
		memcpy(v->coord, v->newCoord, sizeof(float) * 3);
	}
	for (int i = 0; i <m->edges.size(); i++)
	{
		m->edges[i]->mark = 0;
	}
	return;
}

struct model *make_cube()
{
	
	model *m = new model();
	for (int i = 0; i < 8; i++)
	{
		vertex *v = new vertex();
		memcpy(v->coord, &cube_vertices[3 * i], sizeof(float) * 3);
		v->mark = 0;
		m->verts.push_back(v);
	}
	for (int i = 0; i < 12;i++) {
		face *f = new face();
		m->faces.push_back(f);

		edge *e1 = edge_new(f, m->verts[cube_elements[3*i]], m->verts[cube_elements[3 * i + 1]]);
		edge *e2 = edge_new(f, m->verts[cube_elements[3 * i + 1]], m->verts[cube_elements[3 * i + 2]]);
		edge *e3 = edge_new(f, m->verts[cube_elements[3 * i + 2]], m->verts[cube_elements[3 * i]]);
		m->edges.push_back(e1);
		m->edges.push_back(e2);
		m->edges.push_back(e3);

		e1->next = e2;
		e2->next = e3;
		e3->next = e1;
		e1->face = f;
		e2->face = f;
		e3->face = f;
		f->edge = e1;

	}
	find_twins(m->edges);
	subdivide(m);
	//subdivide(m);

	glGenBuffers(1, &m->vbo);
	return m;
}