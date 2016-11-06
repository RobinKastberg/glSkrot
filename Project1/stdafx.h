// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define GLEW_STATIC
#define WIN32_LEAN_AND_MEAN 
#define _CRT_SECURE_NO_WARNINGS
#include "Dependencies/glew/glew.h"
#include "Dependencies/glew/wglew.h"
            // Exclude rarely-used stuff from Windows headers
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
#include "snow.h"
#include "quad.h"
#include "hash.h"
#include "object.h"
#include "shaders_include.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void texture(GLuint *handle, GLenum format, int width, int height, GLenum *params);
enum {BUFFER_FLOAT, BUFFER_DEPTH, BUFFER_COLOR, BUFFER_MONO} trolol;

float perlin2d(float x, float y, float freq, int depth);

unsigned short inline hashfunction(register int key);
void hash_init(struct _hash *h);
void hash_set(struct _hash *h, int key, short value);
int hash_get(struct _hash *h, int key);

struct pprocess
{
	struct object object;
	GLuint fbo;
	GLuint in_texture;
	GLuint out_texture;
	struct shader_program *sp;
	quad q;
	int width;
	int height;
};
void pprocess_new(struct pprocess *self, int width, int height, struct shader_program *sp);
void pprocess_size(struct pprocess *self, int width, int height);
GLuint pprocess_do(struct pprocess *self, GLuint in_texture);

#define NAME(type,id,name) if (GLEW_KHR_debug) {	glObjectLabel(type, id, strlen(name), name);}
__declspec(align(16)) struct global {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 lightMatrix;
	__declspec(align(16)) vec4 cameraPosition;
	__declspec(align(16)) vec4 lookAt;
	__declspec(align(16)) vec4 lightPos;
	__declspec(align(16)) float time;
	__declspec(align(16)) float deltaTime;
};

extern struct global globals;
void debug_init();
float perlin2d(float x, float y, float freq, int depth);
float perlin3d(float x, float y, float z, float freq, int depth);




#define CRASH(str) MessageBoxA(NULL, str, __FILE__, MB_OK | MB_ICONWARNING)