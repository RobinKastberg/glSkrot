#include "stdafx.h"


#define WIREFRAME false
#define POW2(x) (1 << (x))


struct perModel models[MAX_MODELS];
int freePerModel = 0;

void mesh_new(struct mesh *self, unsigned int numVerts, unsigned int numIndices)
{
	glGenVertexArrays(1, &self->vao);
	glGenBuffers(1, &self->vbo);
	glGenBuffers(1, &self->ibo);
	self->data = (struct meshData *)malloc(sizeof(struct meshData)*numVerts);
	self->numVerts = numVerts;
	self->numIndices = numIndices;
	self->indices = (unsigned int *)malloc(sizeof(unsigned int)*numIndices);
	self->uniformIndex = freePerModel++;
	mat4_identity(&models[self->uniformIndex].modelMatrix);
}
void mesh_prepare(struct mesh *self)
{
	glBindVertexArray(self->vao);
	glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
	glBufferData(GL_ARRAY_BUFFER, self->numVerts*sizeof(meshData), self->data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, self->numIndices * sizeof(int), self->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void *)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *)24);

}
// num indices = (4^(MAX_LOD)-1)/3
// MAX_LOD = log(

void square(struct quad *self, int x, int y, int size, int ptr_l)
{
	self->mesh.indices[ptr_l++] = y*self->width + x;
	self->mesh.indices[ptr_l++] = y*self->width + (x + size);
	self->mesh.indices[ptr_l++] = (y + size)*self->width + (x + size);
	self->mesh.indices[ptr_l++] = (y + size)*self->width + x;
}
void lod(struct quad *self, int x, int y, int size, int l)
{
	int ptr = 0;
	for (int lod = 0; lod <= self->max_lod; lod++)
	{
		int dx = POW2(self->max_lod - lod);
		int dy = POW2(self->max_lod - lod);

		for (int i = 0; i < POW2(self->max_lod); i+=dx)
		{
			for (int j = 0; j < POW2(self->max_lod); j+=dy)
			{
				self->mesh.indices[ptr++] = i*self->width + j;
				self->mesh.indices[ptr++] = i*self->width + (j + dx);
				self->mesh.indices[ptr++] = (i + dy)*self->width + (j + dx);
				self->mesh.indices[ptr++] = (i + dy)*self->width + j;
			}
		}

		printf("ptr = %d\n", ptr);
	}
	/*
	self->mesh.indices[ptr++] = y*self->width + x;
	self->mesh.indices[ptr++] = y*self->width + (x + size);
	self->mesh.indices[ptr++] = (y+size)*self->width + (x+size);
	self->mesh.indices[ptr++] = (y+size)*self->width + x;
	 (l <= self->max_lod) {
		square(self, 0, 0, size / 2, ptr);
		square(self, size/2, 0, size / 2, ptr+4);
		square(self, 0, size/2, size / 2, ptr+8);
		square(self, size/2, size/2, size / 2, ptr+12);

		lod(self, 0, 0, size / 2, l + 1);
		lod(self, size / 2, 0, size / 2, l + 1);
		lod(self, 0, size / 2, size / 2, l + 1);
		lod(self, size / 2, size / 2, size / 2, l + 1);
	}*/
}
#define NOISE(x,y) 0.3*perlin2d(xoff + (float)(x) /(self->width-1),yoff + (float)(y) / (self->height-1), 8, 10)

void quad_new(struct quad *self, unsigned int max_lod, float xoff , float yoff )
{
	self->width = POW2(max_lod) + 1;
	self->height = POW2(max_lod) + 1;
	self->max_lod = max_lod;
	mesh_new((struct mesh *)self, self->width*self->height, 4 * (((4 << 2*(max_lod)) - 1)/3));
	for (int i = 0; i < self->width; i++)
	{
		for (int j = 0; j < self->height; j++)
		{
			self->mesh.data[i*self->width + j].vertex.x = xoff + (float)j / (self->width-1);
			self->mesh.data[i*self->width + j].vertex.y = yoff + (float)i / (self->height-1);
			self->mesh.data[i*self->width + j].vertex.z = NOISE(j, i);
		}
	}

	for (int i = 0; i < (self->width); i++)
	{
		for (int j = 0; j < (self->height); j++)
		{
			/*float tr = self->mesh.data[(i + 1)*self->width + j + 1].vertex.z;
			float t = self->mesh.data[(i + 1)*self->width + j].vertex.z;
			float tl = self->mesh.data[(i + 1)*self->width + j - 1].vertex.z;

			float l = self->mesh.data[(i)*self->width + j - 1].vertex.z;
			float r = self->mesh.data[(i)*self->width + j + 1].vertex.z;

			float bl = self->mesh.data[(i - 1)*self->width + j - 1].vertex.z;
			float b = self->mesh.data[(i - 1)*self->width + j].vertex.z;
			float br = self->mesh.data[(i - 1)*self->width + j + 1].vertex.z;*/
			vec3 dx = vec3_new(2.0 / (self->width-1), 0.0,NOISE(j+1, i) - NOISE(j-1,i));
			vec3 dy = vec3_new(0.0, 2.0 / (self->height-1), NOISE(j, i+1) - NOISE(j,i-1));
			vec3 normal;
			vec3_cross(&dx, &dy, &normal);
			//vec3 normal = vec3_new(tr + 2 * r + br - (tl + 2 * l + bl), (tl + 2 * t + tr) - (bl + 2 * b + br), 1.0/100);
			vec3_normalize(&normal);
			self->mesh.data[i*self->width + j].normal = normal;
		}
	}

	lod(self, 0,0, self->width-1, 0);



	SHADER(self->mesh.sp, "quad", standard, quad);

	//shader_init(&quadp, "quad");
	//shader_source(&quadp, GL_FRAGMENT_SHADER, quad_frag, quad_frag_len);
	//shader_source(&quadp, GL_VERTEX_SHADER, standard_vert, standard_vert_len);
	glUniform1i(glGetUniformLocation(self->mesh.sp.program, "tex"), 1);

	mesh_prepare((struct mesh *)self);

}
void quad_draw(struct quad *self)
{
	glBindVertexArray(self->mesh.vao);
	glUseProgram(self->mesh.sp.program);
	glUniform1i(glGetUniformLocation(self->mesh.sp.program, "currentModel"), self->mesh.uniformIndex);
	shader_verify(&self->mesh.sp);
	//glDrawArrays(GL_QUADS, 0, WIDTH*HEIGHT * 4);
	if (self->mesh.wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(5, 5);
		glLineWidth(2.0);
		glDrawElements(GL_QUADS, self->mesh.numIndices, GL_UNSIGNED_INT, NULL);
		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if(self->lod == 0)
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
	else {
		// 4^lod							// sum_0^{lod-1} 4^k 

		int numElements = 4 << 2 * ((int)self->lod);
		int offset = 4 * ((4 << 2*((int)self->lod-1)) - 1) / 3;
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_QUADS, numElements, GL_UNSIGNED_INT, (void *)(offset * sizeof(int)));
		if (self->lod < self->max_lod) {
			int numElements = 4 << 2 * ((int)self->lod+1);
			int offset = 4 * ((4 << 2 * ((int)self->lod)) - 1) / 3;
			//glBlendColor(0, 0, 0, fmod(self->lod, 1.0f));
			//glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
			glDrawElements(GL_QUADS, numElements, GL_UNSIGNED_INT, (void *)(offset * sizeof(int)));
		}
		glPopAttrib();
		//4 * ((4 << max_lod + 1) - 1) / 3
		//GLuint query;
		//glGenQueries(1, &query);
		//glBeginQuery(GL_SAMPLES_PASSED, query);
		
		/*glEndQuery(GL_SAMPLES_PASSED);
		unsigned int pixels;
		glGetQueryObjectuiv(query, GL_QUERY_RESULT, &pixels);
		glDeleteQueries(1, &query);
		printf("%d elements at %d offset\n", numElements, offset);
		printf("%d pixels\n", pixels);
		if (pixels > 50000)
			self->lod = self->max_lod;
		else
			self->lod = 3;*/
		//glDrawElements(GL_QUADS, 4 << 2 * (self->lod), GL_UNSIGNED_INT, (void *)(sizeof(unsigned int)* 4 * ((4 << (2 * (self->lod - 1))) / 3)));
	}
	//glDrawElements(GL_QUADS, 4*4, GL_UNSIGNED_INT, (void *)(4*sizeof(unsigned int)));
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_POLYGON_OFFSET_FILL);
}
