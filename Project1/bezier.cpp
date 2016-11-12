#include "stdafx.h"
static inline float B(int i, float u)
{
	float invsqr = (1.0f - u)*(1.0f-u);
	float inv = 1.0f - u;
	float result;
	switch (i)
	{
		case 0:
			return invsqr;
		case 1:
			return 2*inv*u;
		case 2:
			return u*u;
	}
	abort();
}
static inline float Bprime(int i, float u)
{
	float invsqr = (1.0f - u)*(1.0f - u);
	float inv = 1.0f - u;
	float result;
	switch (i)
	{
	case 0:
		return -2*inv;
	case 1:
		return 2 - 4*u;
	case 2:
		return 2*u;
	}
	abort();
}
void curve_new(struct curve *self, unsigned int steps)
{
	self->steps = steps;
	mesh_new(&self->mesh, steps, 2*(steps-1));

	SHADER(self->mesh.sp, "curve", standard, standard);
	
}
void surface_new(struct surface *self, unsigned int steps)
{
	self->steps = steps;
	mesh_new(&self->mesh, steps * steps, 4 * (steps - 1) * (steps - 1));

	SHADER(self->mesh.sp, "curve", standard, standard);

}
vec3 curve_get_position(struct curve *self, float u)
{
	return vec3{
		B(0,u)*self->p[0].x + B(1,u)*self->p[1].x + B(2,u)*self->p[2].x,
		B(0,u)*self->p[0].y + B(1,u)*self->p[1].y + B(2,u)*self->p[2].y,
		B(0,u)*self->p[0].z + B(1,u)*self->p[1].z + B(2,u)*self->p[2].z
	};
}
vec3 surface_get_position(struct surface *self, float u, float v)
{
	return vec3{
		B(0,u)*B(0, v)*self->p[0].x + B(0,u)*B(1, v)*self->p[1].x + B(0,u)*B(2, v)*self->p[2].x
	+   B(1,u)*B(0, v)*self->p[3].x + B(1,u)*B(1, v)*self->p[4].x + B(1,u)*B(2, v)*self->p[5].x
	+   B(2,u)*B(0, v)*self->p[6].x + B(2,u)*B(1, v)*self->p[7].x + B(2,u)*B(2, v)*self->p[8].x,

		B(0,u)*B(0, v)*self->p[0].y + B(0,u)*B(1, v)*self->p[1].y + B(0,u)*B(2, v)*self->p[2].y
		+ B(1,u)*B(0, v)*self->p[3].y + B(1,u)*B(1, v)*self->p[4].y + B(1,u)*B(2, v)*self->p[5].y
		+ B(2,u)*B(0, v)*self->p[6].y + B(2,u)*B(1, v)*self->p[7].y + B(2,u)*B(2, v)*self->p[8].y,

		B(0,u)*B(0, v)*self->p[0].z + B(0,u)*B(1, v)*self->p[1].z + B(0,u)*B(2, v)*self->p[2].z
		+ B(1,u)*B(0, v)*self->p[3].z + B(1,u)*B(1, v)*self->p[4].z + B(1,u)*B(2, v)*self->p[5].z
		+ B(2,u)*B(0, v)*self->p[6].z + B(2,u)*B(1, v)*self->p[7].z + B(2,u)*B(2, v)*self->p[8].z

	};
}
vec3 surface_du(struct surface *self, float u, float v)
{
	return vec3{
		Bprime(0,u)*B(0, v)*self->p[0].x + Bprime(0,u)*B(1, v)*self->p[1].x + Bprime(0,u)*B(2, v)*self->p[2].x
		+ Bprime(1,u)*B(0, v)*self->p[3].x + Bprime(1,u)*B(1, v)*self->p[4].x + Bprime(1,u)*B(2, v)*self->p[5].x
		+ Bprime(2,u)*B(0, v)*self->p[6].x + Bprime(2,u)*B(1, v)*self->p[7].x + Bprime(2,u)*B(2, v)*self->p[8].x,

		Bprime(0,u)*B(0, v)*self->p[0].y + Bprime(0,u)*B(1, v)*self->p[1].y + Bprime(0,u)*B(2, v)*self->p[2].y
		+ Bprime(1,u)*B(0, v)*self->p[3].y + Bprime(1,u)*B(1, v)*self->p[4].y + Bprime(1,u)*B(2, v)*self->p[5].y
		+ Bprime(2,u)*B(0, v)*self->p[6].y + Bprime(2,u)*B(1, v)*self->p[7].y + Bprime(2,u)*B(2, v)*self->p[8].y,

		Bprime(0,u)*B(0, v)*self->p[0].z + Bprime(0,u)*B(1, v)*self->p[1].z + Bprime(0,u)*B(2, v)*self->p[2].z
		+ Bprime(1,u)*B(0, v)*self->p[3].z + Bprime(1,u)*B(1, v)*self->p[4].z + Bprime(1,u)*B(2, v)*self->p[5].z
		+ Bprime(2,u)*B(0, v)*self->p[6].z + Bprime(2,u)*B(1, v)*self->p[7].z + Bprime(2,u)*B(2, v)*self->p[8].z

	};
}
vec3 surface_dv(struct surface *self, float u, float v)
{
	return vec3{
		B(0,u)*Bprime(0,v)*self->p[0].x + B(0,u)*Bprime(1,v)*self->p[1].x + B(0,u)*Bprime(2,v)*self->p[2].x
		+ B(1,u)*Bprime(0,v)*self->p[3].x + B(1,u)*Bprime(1,v)*self->p[4].x + B(1,u)*Bprime(2,v)*self->p[5].x
		+ B(2,u)*Bprime(0,v)*self->p[6].x + B(2,u)*Bprime(1,v)*self->p[7].x + B(2,u)*Bprime(2,v)*self->p[8].x,

		B(0,u)*Bprime(0,v)*self->p[0].y + B(0,u)*Bprime(1,v)*self->p[1].y + B(0,u)*Bprime(2,v)*self->p[2].y
		+ B(1,u)*Bprime(0,v)*self->p[3].y + B(1,u)*Bprime(1,v)*self->p[4].y + B(1,u)*Bprime(2,v)*self->p[5].y
		+ B(2,u)*Bprime(0,v)*self->p[6].y + B(2,u)*Bprime(1,v)*self->p[7].y + B(2,u)*Bprime(2,v)*self->p[8].y,

		B(0,u)*Bprime(0,v)*self->p[0].z + B(0,u)*Bprime(1,v)*self->p[1].z + B(0,u)*Bprime(2,v)*self->p[2].z
		+ B(1,u)*Bprime(0,v)*self->p[3].z + B(1,u)*Bprime(1,v)*self->p[4].z + B(1,u)*Bprime(2,v)*self->p[5].z
		+ B(2,u)*Bprime(0,v)*self->p[6].z + B(2,u)*Bprime(1,v)*self->p[7].z + B(2,u)*Bprime(2,v)*self->p[8].z

	};
}
void curve_prepare(struct curve *self)
{
	for (int i = 0; i < self->steps; i++)
	{
		float u = (float)i / (self->steps - 1);
		self->mesh.data[i].vertex = curve_get_position(self, u);
	}
	for (int i = 0; i < self->steps-1; i++)
	{
		self->mesh.indices[2 * i + 0] = i;
		self->mesh.indices[2 * i + 1] = i+1;
	}
	mesh_prepare(&self->mesh);
}

void surface_prepare(struct surface *self)
{
	for (int i = 0; i < self->steps; i++)
	{
		for (int j = 0; j < self->steps; j++)
		{
			float u = (float)i / (self->steps - 1);
			float v = (float)j / (self->steps - 1);
			self->mesh.data[i*self->steps + j].vertex = surface_get_position(self, u, v);
			vec3 du = surface_du(self, u, v);
			vec3 dv = surface_dv(self, u, v);
			vec3_normalize(&du);
			vec3_normalize(&dv);
			self->mesh.data[i*self->steps + j].normal = vec3_cross(&dv, &du);
		}
	}
	unsigned int ptr = 0;
	for (int i = 0; i < self->steps - 1; i++)
	{
		for (int j = 0; j < self->steps - 1; j++)
		{
			self->mesh.indices[ptr++] = i*self->steps + j;
			self->mesh.indices[ptr++] = i*self->steps + (j + 1);
			self->mesh.indices[ptr++] = (i + 1)*self->steps + (j + 1);
			self->mesh.indices[ptr++] = (i + 1)*self->steps + j;
		}
	}


	mesh_prepare(&self->mesh);
}

void curve_draw(struct curve *self)
{
	mesh_draw(&self->mesh, GL_LINES, 2*(self->steps-1), 0);
}
void surface_draw(struct surface *self)
{
	mesh_draw(&self->mesh, GL_QUADS, 4 * (self->steps - 1) * (self->steps - 1), 0);
}