// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define GLEW_STATIC
#include "Dependencies/glew/glew.h"
#include "Dependencies/glew/wglew.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <assert.h>
#include <memory.h>

// C++
#include <algorithm>
#include <map>
// TODO: reference additional headers your program requires here
#include "shader.h"
#include "chunk.h"
#include "model.h"
#include "transform.h"

struct xyz {
	float x, y, z;
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void texture(GLuint *handle, GLenum format, int width, int height, GLenum *params);
enum {BUFFER_FLOAT, BUFFER_DEPTH, BUFFER_COLOR, BUFFER_MONO} trolol;


unsigned short inline hashfunction(register int key);
void hash_init(struct _hash *h);
void hash_set(struct _hash *h, int key, short value);
int hash_get(struct _hash *h, int key);

struct pprocess
{
	GLuint fbo;
	GLuint in_texture;
	GLuint out_texture;
	struct shader_program *sp;
	int width;
	int height;
};
void pprocess_new(struct pprocess *self, int width, int height, struct shader_program *sp);
void pprocess_size(struct pprocess *self, int width, int height);
GLuint pprocess_do(struct pprocess *self, GLuint in_texture);

struct global {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 lightMatrix;
	vec3 cameraPosition;
	vec3 lookAt;
	vec3 lightPos;
};

extern struct global globals;
enum {BUFFER_FLOAT, BUFFER_DEPTH, BUFFER_COLOR, BUFFER_MONO};

struct xyz {
	float x;
	float y;
	float z;
};

#define CRASH(str) MessageBoxA(NULL, str, __FILE__, MB_OK | MB_ICONWARNING)