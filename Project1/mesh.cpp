#include "stdafx.h"

struct perModel models[MAX_MODELS];
static int freePerModel = 0;

void mesh_new(struct mesh *self, unsigned int numVerts, unsigned int numIndices)
{
	self->data = (struct meshData *)malloc(sizeof(struct meshData)*numVerts);
	self->numVerts = numVerts;
	self->numIndices = numIndices;
	self->indices = (unsigned int *)malloc(sizeof(unsigned int)*numIndices);
	self->uniformIndex = freePerModel++;
	mat4_scale(&models[self->uniformIndex].modelMatrix, 1.0);

	glGenVertexArrays(1, &self->vao);
	glGenBuffers(1, &self->vbo);
	glGenBuffers(1, &self->ibo);
	glBindVertexArray(self->vao);
	glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
	glBufferData(GL_ARRAY_BUFFER, self->numVerts*sizeof(meshData), NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void *)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *)24);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, self->numIndices * sizeof(int), NULL, GL_STATIC_DRAW);
}
void mesh_prepare(struct mesh *self)
{
	glBindVertexArray(self->vao);
	glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, self->numVerts*sizeof(meshData), self->data);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,  self->numIndices * sizeof(int), self->indices);
}
void mesh_draw(struct mesh *self, GLenum type, unsigned int count, unsigned int offset)
{
	glBindVertexArray(self->vao);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	shader_use(&self->sp);
	glUniform1i(glGetUniformLocation(self->sp.program, "currentModel"), self->uniformIndex);
	shader_verify(&self->sp);
	if (self->wireframe) {
		glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT);
		glPolygonOffset(5, 5);
		glLineWidth(2.0);
		glDisable(GL_CULL_FACE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
		
	glDrawElements(type, count, GL_UNSIGNED_INT, (void *)(offset * sizeof(int)));

	if (self->wireframe) {
		glPopAttrib();
	}
}