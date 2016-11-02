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


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
double noise(double x, double y, double z);
void permute();

struct xyz {
	float x;
	float y;
	float z;
};

#define CRASH(str) MessageBoxA(NULL, str, __FILE__, MB_OK | MB_ICONWARNING)