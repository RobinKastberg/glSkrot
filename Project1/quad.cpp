#include "stdafx.h"
static struct quad *q = NULL;
static float const quadMesh[] = {
	-1.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f,

	1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f,

	-1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f
};
void quad_new(struct quad *self)
{
	mesh_new(&self->mesh, 4, 4);
	memcpy(self->mesh.data, quadMesh, sizeof(quadMesh));
}
void quad_draw(struct quad *self)
{
	mesh_draw(&self->mesh, GL_QUADS, 4, 0);
}
void draw_quad(struct shader_program *sp)
{
	if (q == NULL)
	{
		q = (struct quad *)malloc(sizeof(struct quad));
		quad_new(q);
	}
	q->mesh.sp = *sp;
	mesh_draw(&q->mesh, GL_QUADS, 4, 0);
}