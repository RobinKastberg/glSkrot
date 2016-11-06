// Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <windows.h>
#include <GL/GL.h>
#include "model.h"
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "Dependencies/glew/glew32s.lib")

struct model *cube;
#define MRTS 4
static HDC hdc;
static HGLRC hglrc;
static HWND hwnd;
static int width;
static int height;
static int cameraLoc, tex1Loc, tex2Loc, tex3Loc, tex4Loc, shadow2Loc, shadowLoc, lightLoc;
struct global globals;
struct shader_program sp;

struct shader_program shadowp;
struct shader_program blurp;
static GLuint m_vaoID[2];
static GLuint m_vboID[3];
static GLuint uboGlobals, uboModels;
static GLuint fbos[2];
static GLuint texs[MRTS+2];
static superchunk *cnk;

float thetax = 0;
float thetay = 3.14159/4;
float radius = 8.0;

quad q;
quad q2;
void mouse_move(int dx, int dy)
{
	thetax += ((float)dx)/400.0;
	thetay -= ((float)dy)/400.0;
	thetay = max(min(thetay, 3.14), -3.14);
	globals.cameraPosition.x = radius * cos(thetax)*sin(thetay) + globals.lookAt.x;
	globals.cameraPosition.y = radius * sin(thetax)*sin(thetay) + globals.lookAt.y;
	globals.cameraPosition.z = radius * cos(thetay) + globals.lookAt.z;
}




HINSTANCE hInstance;
struct pprocess blur;
struct pprocess blur2;
void init()
{
	wglSwapIntervalEXT(1);
	glClearColor(0, 0, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mouse_move(0, 0);
	//init_snow();
	//init_quad();
	quad_new(&q, 8);
	q.lod = 2;
	quad_new(&q2, 8,1,0);
	q2.lod = 2;

	q.mesh.wireframe = false;
	init_skybox();
	glGenBuffers(1, &uboGlobals);
	glBindBuffer(GL_UNIFORM_BUFFER, uboGlobals);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(globals), &globals, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboGlobals);

	glGenBuffers(1, &uboModels);
	glBindBuffer(GL_UNIFORM_BUFFER, uboModels);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(models), &models, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboModels);

	globals.lookAt = vec4{ 5,5,1,0 };


}


void render()
{
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4_perspective(&globals.projectionMatrix, 90, (float)width / height, 0.01, 5000);
	struct vec3 up = vec3_new(0, 0, 1);
	mat4_lookat(&globals.viewMatrix, (vec3 *)&globals.cameraPosition, (vec3 *)&globals.lookAt, &up);


	glBindBuffer(GL_UNIFORM_BUFFER, uboGlobals);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(globals), &globals);
	glBindBuffer(GL_UNIFORM_BUFFER, uboModels);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(models), &models);

	//draw_snow();
	draw_skybox();

	q.lod = 16.0/sqrt((globals.cameraPosition.x - 5)*(globals.cameraPosition.x - 5)
		+ (globals.cameraPosition.y - 5)*(globals.cameraPosition.y - 5)
		+ (globals.cameraPosition.z - 1)*(globals.cameraPosition.z - 1));
	q.lod = max(min(q.lod, 8), 0);

	q2.lod = 16.0 / sqrt((globals.cameraPosition.x - 15)*(globals.cameraPosition.x - 15)
		+ (globals.cameraPosition.y - 5)*(globals.cameraPosition.y - 5)
		+ (globals.cameraPosition.z - 1)*(globals.cameraPosition.z - 1));
	q2.lod = max(min(q2.lod, 8), 0);


	quad_draw(&q);
	quad_draw(&q2);
}

void __stdcall WinMainCRTStartup() {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	int nshowcmd = SW_SHOWDEFAULT;
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"oglversionchecksample";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.style = CS_OWNDC;
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
	}
	hwnd = CreateWindowW(wc.lpszClassName, L"jag vet inte vad", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	init();
	debug_init();
	ShowWindow(hwnd, nshowcmd);
	UpdateWindow(hwnd);
	unsigned int lastTime = GetTickCount();
	unsigned int lastUpdate = GetTickCount();
	int nbFrames = 0;
	char fpsString[50];

	SetCapture(hwnd);

	for(;;)
	{
		// Measure speed
		unsigned int currentTime = GetTickCount();
		nbFrames++;
		if (currentTime - lastTime >= 1000) // If last prinf() was more than 1 sec ago
		{
			// printf and reset timer
			snprintf(fpsString, 50, "%f FPS\n", double(nbFrames));
			OutputDebugStringA(fpsString);
			nbFrames = 0;
			lastTime = GetTickCount();
			
		}
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           // Is There A Message Waiting?
		{
			TranslateMessage(&msg);             // Translate The Message
			DispatchMessage(&msg);              // Dispatch The Message
		}
		if (msg.message == WM_QUIT)
			break;

		
		globals.time += 0.001f*(currentTime - lastUpdate);
		globals.deltaTime = 0.001f*(currentTime - lastUpdate);
		lastUpdate = GetTickCount();
		render();
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			OutputDebugStringA((char *)gluErrorString(err));
		}
		SwapBuffers(hdc);
	}
	return;
}
POINT g_OrigCursorPos;
POINT g_OrigWndPos;
bool g_MovingMainWnd = false;
int current_x = 0;
int current_y = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	case WM_CREATE:
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
			32,                        //Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,						// number of bits for accumulation buffer
			0, 0, 0, 0,
			24,                        //Number of bits for the depthbuffer
			8,                        //Number of bits for the stencilbuffer
			0,                        //Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};
		hdc = GetDC(hWnd);

		int  letWindowsChooseThisPixelFormat;
		letWindowsChooseThisPixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, letWindowsChooseThisPixelFormat, &pfd);

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
			WGL_CONTEXT_MINOR_VERSION_ARB, 0,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB, 
			0
		};
		
		hglrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hglrc);	
		glewInit();
		hglrc = wglCreateContextAttribsARB(hdc, 0, attribs);
		assert(hglrc);
		wglMakeCurrent(NULL, NULL);
		wglMakeCurrent(hdc, hglrc);
		OutputDebugStringA((char *)glGetString(GL_VERSION));
		OutputDebugStringA((char *)glGetString(GL_RENDERER));
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		// resize the viewport after the window had been resized
		//Resize(LOWORD(lparam), HIWORD(lparam));
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		glViewport(0, 0,  width, height);

		break;
	}
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return 0;

	case WM_CAPTURECHANGED:
		g_MovingMainWnd = (HWND)lParam == hWnd;
		return 0;
	case WM_MOUSEWHEEL:
		radius += (float)(short)HIWORD(wParam)/120.0;
		radius = min(max(radius, 3), 20);
		mouse_move(0, 0);
		break;
	case WM_MOUSEMOVE:
		if (g_MovingMainWnd)
		{
			POINT pt;
			if (GetCursorPos(&pt))
			{
				int wnd_x = 
					(pt.x - g_OrigCursorPos.x);
				int wnd_y =
					(pt.y - g_OrigCursorPos.y);
				mouse_move(wnd_x, wnd_y);
				SetCursorPos(g_OrigCursorPos.x, g_OrigCursorPos.y);
			}
		} else {
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd, &pt);
			struct mat4 inverseView;
			struct mat4 inverseProjection;
			mat4_inverse((float *)&globals.viewMatrix, (float *)&inverseView);
			mat4_inverse((float *)&globals.projectionMatrix, (float *)&inverseProjection);
			struct vec4 ray = { (float)2 * pt.x / width - 1, 1 - (float)2 * pt.y / height , -1, 1};
			vec4 projectionRay = mat4_mul(&inverseProjection, &ray);
			projectionRay.z = -1;
			projectionRay.w = 0;
			vec4 viewRay = mat4_mul(&inverseView, &projectionRay);
			float coeff = (1.5-globals.cameraPosition.z) / viewRay.z;
			float x = globals.cameraPosition.x + coeff*viewRay.x;
			float y = globals.cameraPosition.y + coeff*viewRay.y;
			int minx;
			int miny;
			float dist = FLT_MAX;
			for (float i = 0; i < 16; i++)
			{
				for (float j = 0; j < 16; j++)
				{
					float dist2 = sqrtf((i - x + 0.5)*(i - x + 0.5) + (j - y  + 0.5)*(j - y + 0.5));
					
					if (dist2 < dist)
					{
						minx = i;
						miny = j;
						dist = dist2;
					}
				}
			}
			//if (minx > 1 && minx < 15 && miny > 1 && miny < 15 && !cnk->get(minx, miny, 2)) {
				//cnk->set(current_x, current_y, 2, 0);
				//cnk->set(minx, miny, 2, 1);
				current_x = minx;
				current_y = miny;
			//}
			return 0;
		}
	case WM_SYSKEYDOWN:
		return 0;
		break;
	case WM_LBUTTONDOWN:
		current_x = -1;
		current_y = -1;
		break;
	case WM_RBUTTONDOWN:
		// here you can add extra check and decide whether to start
		// the window move or not
		if (GetCursorPos(&g_OrigCursorPos))
		{
			RECT rt;
			GetWindowRect(hWnd, &rt);
			g_MovingMainWnd = true;
			SetCapture(hWnd);
			SetCursor(LoadCursor(NULL, NULL));
		}
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int nshowcmd)
{
	WinMainCRTStartup();
}

void main() {
	WinMainCRTStartup();
}