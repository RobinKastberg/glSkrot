#include "stdafx.h"


extern struct shader_program sp;
extern struct xyz {
	float x;
	float y;
	float z;
} cameraPosition, lookAt;

superchunk::superchunk() {
	memset(c, 0, sizeof c);
}

superchunk::~superchunk() {
	for (int x = 0; x < SCX; x++)
		for (int y = 0; y < SCX; y++)
			for (int z = 0; z < SCX; z++)
				delete c[x][y][z];
}

GLubyte superchunk::get(int x, int y, int z) const {
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;

	if (!c[cx][cy][cz])
		return 0;
	else
		return c[cx][cy][cz]->get(x, y, z);
}

void superchunk::set(int x, int y, int z, GLubyte type) {
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;

	if (!c[cx][cy][cz])
		c[cx][cy][cz] = new chunk();

	c[cx][cy][cz]->set(x, y, z, type);
}

void superchunk::render() {

	struct xyz fudgedCamera;
	fudgedCamera.x = cameraPosition.x;
	fudgedCamera.y = cameraPosition.y;
	float len = sqrtf((lookAt.x - cameraPosition.x)*(lookAt.x - cameraPosition.x) +
		(lookAt.y - cameraPosition.y)*(lookAt.y - cameraPosition.y));
	fudgedCamera.x -= CX * (lookAt.x - cameraPosition.x) / len;
	fudgedCamera.y -= CY * (lookAt.y - cameraPosition.y) / len;
	for (int x = 0; x < SCX; x++)
		for (int y = 0; y < SCY; y++)
			for (int z = 0; z < SCZ; z++)
				if (c[x][y][z]
					&& (x* CX - cameraPosition.x)* (x* CX - cameraPosition.x)
					  + (y*CY - cameraPosition.y)* (y*CY - cameraPosition.y) < 300*300
					&& ((x * CX - fudgedCamera.x)* (lookAt.x - fudgedCamera.x) + 
					(y * CY - fudgedCamera.y)* (lookAt.y - fudgedCamera.y)) > 0) {
					//glm::translate(glm::mat4(1), glm::vec3(x * CX, y * CY, z * CZ));
					glPushMatrix();
					glVertexAttrib3f(2, x * CX, y * CY, z * CZ);
					glTranslatef(x * CX, y * CY, z * CZ);
					c[x][y][z]->render();
					glPopMatrix();
				}
}

void superchunk::update() {
	for (int x = 0; x < SCX; x++)
		for (int y = 0; y < SCY; y++)
			for (int z = 0; z < SCZ; z++)
				if (c[x][y][z])
					c[x][y][z]->update();
}