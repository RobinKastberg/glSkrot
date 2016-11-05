#include "stdafx.h"

struct edge *edge_new(model *m, struct face *f, int v0i, int v1i)
{
	assert(v0i < m->verts.size());
	assert(v1i < m->verts.size());
	struct edge *e = new edge();
	m->verts[v0i]->out = e;
	e->v0 = m->verts[v0i];
	e->v0i = v0i;
	e->v1 = m->verts[v1i];
	e->v1i = v1i;
	e->face = f;
	return e;
}
void chkdsk(struct model *m)
{

#ifdef _DEBUG
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
#endif
}
static inline int pair(register short i, register short j)
{
	return ((int)i << 16) | (j & 0xFFFF);
}
void find_twins(std::vector<edge *> edges)
{
	struct hash h;
	hash_init(&h);
	for (int i = 0; i < edges.size(); i++)
		hash_set(&h, pair(edges[i]->v0i, edges[i]->v1i), i);
		//Edges[std::make_pair(edges[i]->v0, edges[i]->v1)] = edges[i];
	for (int i = 0; i < edges.size(); i++)
		if (edges[i]->pair == NULL)
		{
			//edge *pair = Edges[std::make_pair(edges[i]->v1, edges[i]->v0)];
			unsigned short index = hash_get(&h, pair(edges[i]->v1i, edges[i]->v0i));
			edge *p = edges[index];
			edges[i]->pair = p;
		}
		
}

void draw(struct model *m)
{
	if (m->up_to_date == 0)
	{
		m->up_to_date = 1;
		float *glData = new float[2 * 18 * m->faces.size()];
		for (int i = 0; i < m->faces.size(); i++)
		{
			float normal[3][3];
			memset(normal, 0, 3 * 3 * 4);
#ifdef NORMALS
			face *f = m->faces[i];

			std::vector<vertex *> adjacent;

			edge *e = f->edge;
			for (int j = 0; j < 3; j++) {
				edge *ee = e;
				int facesCount = 0;
				do {
					float N[3];
					facesCount++;
					f = e->face;
					float A[3] = { f->edge->v1->coord[0] - f->edge->v0->coord[0],
									f->edge->v1->coord[1] - f->edge->v0->coord[1],
									f->edge->v1->coord[2] - f->edge->v0->coord[2] };
					float B[3] = { f->edge->next->next->v0->coord[0] - f->edge->next->next->v1->coord[0],
									f->edge->next->next->v0->coord[1] - f->edge->next->next->v1->coord[1],
									f->edge->next->next->v0->coord[2] - f->edge->next->next->v1->coord[2] };
					N[0] = A[1] * B[2] - A[2] * B[1];
					N[1] = A[2] * B[0] - A[0] * B[2];
					N[2] =A[0] * B[1] - A[1] * B[0];
					float nrm = sqrt(N[0]*N[0] + N[1] * N[1] + N[2] * N[2]);
					N[0] /= nrm;
					N[1] /= nrm;
					N[2] /= nrm;
					normal[j][0] += N[0];
					normal[j][1] += N[1];
					normal[j][2] += N[2];
					e = e->pair->next;
				} while (e != ee);
				e = e->next;

				normal[j][0] /= facesCount;
				normal[j][1] /= facesCount;
				normal[j][2] /= facesCount;
			}
		
#endif NORMALS
			memcpy(glData + 18 * i, m->faces[i]->edge->v0->coord, sizeof(float) * 3);
			memcpy(glData + 18 * i + 3, normal[0], sizeof(float) * 3);
			memcpy(glData + 18 * i + 6, m->faces[i]->edge->next->v0->coord, sizeof(float) * 3);
			memcpy(glData + 18 * i + 9, normal[1], sizeof(float) * 3);
			memcpy(glData + 18 * i + 12, m->faces[i]->edge->next->next->v0->coord, sizeof(float) * 3);
			memcpy(glData + 18 * i + 15, normal[2], sizeof(float) * 3);
		}
		

		glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
		glBufferData(GL_ARRAY_BUFFER, 2 * 6 * 3 * m->faces.size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * 6 * 3 * m->faces.size() * sizeof(float), glData);
		if (GLEW_NV_vertex_buffer_unified_memory) {
			m->vbo_size = 2 * 6 * 3 * m->faces.size() * sizeof(float);
			glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &m->vbo_addr);
			glMakeBufferResidentNV(GL_ARRAY_BUFFER, GL_READ_ONLY);
			
		} else {
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void *)12);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
		}
		m->face_count = m->faces.size();
		for (int i = 0; i < m->faces.size();i++)
		{
			delete m->faces[i];
		}
		for (int i = 0; i < m->edges.size(); i++)
		{
			delete m->edges[i];
		}
		for (int i = 0; i < m->verts.size(); i++)
		{
			delete m->verts[i];
		}
		m->faces.clear();
		m->verts.clear();
		m->edges.clear();
	}


	if (glewGetExtension("GL_NV_vertex_buffer_unified_memory")) {
		glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 0, m->vbo_addr, m->vbo_size);
		glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 1, m->vbo_addr, m->vbo_size);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	}
	glDrawArrays(GL_TRIANGLES, 0,  3 *  m->face_count);
}
void smooth(struct model *m)
{
	int vert_len = m->verts.size();
	for (int i = 0; i < vert_len; i++)
	{
		vertex *v = m->verts[i];
		std::vector<vertex *> adjacent;

		edge *e = v->out;
		do {
			adjacent.push_back(e->v1);

			e = e->pair->next;
		} while (e != v->out);
		int N = adjacent.size();
		
		for (int k = 0; k < 3; k++)
		{
			v->newCoord[k] = v->coord[k] / (N+1);
			//if (N > 6)
			//	v->newCoord[k] = 0;
		}
		//if (N > 6)
		//	N--;
		for (int j = 0; j < adjacent.size(); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				v->newCoord[k] += adjacent[j]->coord[k] / (N+1);
			}
		}
	}
	for (int i = 0; i < vert_len; i++)
	{
		vertex *v = m->verts[i];
		memcpy(v->coord, v->newCoord, sizeof(float) * 3);
	}
}
void subdivide(struct model *m, bool loop)
{
	edge **edges = m->edges.data();
	int edge_len = m->edges.size();
	int vert_len = m->verts.size();
	int face_len = m->faces.size();
	chkdsk(m);
	for (int i = 0; i < vert_len; i++)
	{
		vertex *v = m->verts[i];

		if (loop)
		{
			std::vector<vertex *> adjacent;

			edge *e = v->out;
			do {
				adjacent.push_back(e->v1);

				e = e->pair->next;
			} while (e != v->out);
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

		} else {
			for (int k = 0; k < 3; k++)
			{
				v->newCoord[k] = v->coord[k];
			}
		}


	}
	if (loop) {
		for (int i = 0; i < m->edges.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				edges[i]->newCoord[j] = 0.375*edges[i]->v0->coord[j] + 0.375*edges[i]->v1->coord[j]
					+ 0.125*edges[i]->next->v1->coord[j] + 0.125*edges[i]->pair->next->v1->coord[j];
			}
		}
	} else {
		for (int i = 0; i < m->edges.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				edges[i]->newCoord[j] = 0.5*edges[i]->v0->coord[j] + 0.5*edges[i]->v1->coord[j];
				//m->edges[i]->newCoord[j] = 0.5*m->edges[i]->v0->newCoord[j] + 0.5*m->edges[i]->v1->newCoord[j];
			}
		}
	}
//	std::copy(m->edges.begin(), m->edges.end(), oldEdges.begin());
	for (int i = 0; i <edge_len; i++)
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

	for (int i = 0; i < face_len; i++)
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
	vert_len = m->verts.size();
	edge_len = m->edges.size();
	
	
	for (int i = 0; i < vert_len; i++)
	{
		vertex *v = m->verts[i];
		memcpy(v->coord, v->newCoord, sizeof(float) * 3);
	}
	for (int i = 0; i <edge_len; i++)
	{
		m->edges[i]->mark = 0;
	}
	return;
}


void make_model(struct model *m, int *cube_vertices, short *indices, int vsize, int isize)
{

	m->verts.clear();
	m->faces.clear();
	m->edges.clear();
	for (int i = 0; i < vsize; i++)
	{
		vertex *v = new vertex();
		v->coord[0] = cube_vertices[i] & 0xff;
		v->coord[1] = (cube_vertices[i] >> 8) & 0xff;
		v->coord[2] = (cube_vertices[i] >> 16) & 0xff;
		v->mark = 0;
		m->verts.push_back(v);
	}
	for (int i = 0; i < isize/3; i++) {
		face *f = new face();
		m->faces.push_back(f);

		edge *e1 = edge_new(m, f, indices[3 * i], indices[3 * i + 1]);
		edge *e2 = edge_new(m, f, indices[3 * i + 1], indices[3 * i + 2]);
		edge *e3 = edge_new(m, f, indices[3 * i + 2], indices[3 * i]);
		
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
	//subdivide(m, true);
	//smooth(m);
	//subdivide(m, true);
	//smooth(m);
	//smooth(m);
	//subdivide(m , true);
	
	//subdivide(m, true);
	//smooth(m);

	//smooth(m);
	//subdivide(m);
	//subdivide(m);

	m->up_to_date = 0;
#ifdef _DEBUG
	struct hash h;
	hash_init(&h);
	int key = rand();
	short value = rand();
	hash_set(&h, key, value);
	short i = hash_get(&h, key);
	assert(i == value);
#endif
}