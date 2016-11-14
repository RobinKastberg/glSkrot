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
static struct fbo fbo;
float thetax = 0;
float thetay = 3.14159/4;
float radius = 8.0;
static unsigned int animation_step = 0;
terrain q[25];
GLuint ppTex;
static bool variable_work_size = false;
static int work_group_size = 128;
static int blur_passes = 3;
static bool blur_enabled = true;

struct vec3 UP = vec3{ 0, 0, 1 };
struct vec3 DOWN = vec3{ 0, 0, -1 };

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
struct shader_program blur;
struct pprocess blur2;
struct surface s;
struct surface s3;
#define NUM_GRASS 128
surface grass[NUM_GRASS];
void init()
{
	wglSwapIntervalEXT(0);
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

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++) 
		{
			terrain_new(&q[5 * i + j], 5, i, j);
			q[5 * i + j].lod = 1;

		}
	}

	for (int i = 0; i < NUM_GRASS; i++)
	{
		surface_new(&grass[i], 5);
		float baseX = 0.0;
		float baseY = 0.0;
		float rndX = (float)(rand() % 100)/20.0;
		float rndY = (float)(rand() % 100)/20.0;
		float highX = 0.0;
		float baseZ =  0.0;
		grass[i].p[0] = vec3{ baseX-0.15f,baseY,baseZ };
		grass[i].p[1] = vec3{ baseX+0.00f,baseY,baseZ };
		grass[i].p[2] = vec3{ baseX+0.15f,baseY,baseZ };

		grass[i].p[3] = vec3{ baseX + highX / 2 - 0.075f,baseY,baseZ+1.5f };
		grass[i].p[4] = vec3{ baseX + highX / 2,baseY,baseZ + 1.5f };
		grass[i].p[5] = vec3{ baseX + highX / 2 + 0.075f,baseY, baseZ +1.5f };

		grass[i].p[6] = vec3{ baseX + highX - 0.01f,baseY,baseZ+3.0f };
		grass[i].p[7] = vec3{ baseX + highX,baseY,baseZ+3.0f };
		grass[i].p[8] = vec3{ baseX + highX + 0.01f,baseY,baseZ+3.0f };
		mat4 scale;
		mat4 translate;
		mat4 result;
		mat4_scale(&scale,0.1f);
		mat4_translate(&translate, rndX, rndY,NOISE_EX(rndX, rndY)-0.01f);

		mat4_mul(&scale, &translate, &models[grass[i].mesh.uniformIndex].modelMatrix);
		surface_prepare(&grass[i]);
	}
	

	s.mesh.wireframe = true;

	init_skybox();
	glGenBuffers(1, &uboGlobals);
	glBindBuffer(GL_UNIFORM_BUFFER, uboGlobals);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(globals), &globals, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboGlobals);

	glGenBuffers(1, &uboModels);
	glBindBuffer(GL_UNIFORM_BUFFER, uboModels);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(models), &models, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboModels);


	globals.lightPos = vec4{ 2.5,2.5,1,0 };
	fbo_new(&fbo, 2*width, 2*height);
	shader_init(&blur, "blur");
	shader_compute(&blur, blur_comp, blur_comp_len);
}


void render()
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Frame setup");
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4_perspective(&globals.projectionMatrix, 90, (float)width / height, 0.01, 5000);
	//vec3 positionCurve = curve_get_position(&c, fmod(globals.time, 1.0f));
	//globals.lookAt = vec4{ positionCurve.x,positionCurve.y,positionCurve.z,1 };


	mat4_lookat(&globals.viewMatrix, (vec3 *)&globals.cameraPosition, (vec3 *)&globals.lookAt, &UP);


	glBindBuffer(GL_UNIFORM_BUFFER, uboGlobals);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(globals), &globals);
	glBindBuffer(GL_UNIFORM_BUFFER, uboModels);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(models), &models);
	glPopDebugGroup();
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Drawcalls");
	//draw_snow();
	s.steps = 4;
	float t = globals.time;
	fbo_render_to_texture(&fbo);
	for (int i = 0; i < NUM_GRASS; i++)
	{

		mat4 newMatrix;
		vec3 negCam = vec3_neg(globals.lookAt);
		vec3 negLook = vec3_neg(globals.cameraPosition);
		//mat4_lookat(&newMatrix, (vec3 *)&globals.lookAt, (vec3 *)&globals.cameraPosition, &DOWN);
		//mat4_mul(&grass[i].localMatrix, &newMatrix, &models[grass[i].mesh.uniformIndex].modelMatrix);
		grass[i].p[6].x += 0.001*sin(globals.time);
		grass[i].p[7].x += 0.001*sin(globals.time);
		grass[i].p[8].x += 0.001*sin(globals.time);
		glDisable(GL_CULL_FACE);
		//surface_prepare(&grass[i]);
		surface_draw(&grass[i]);
	}


	for (int i = 0; i < 25; i++)
	{
		terrain_draw(q + i);
	}
	draw_skybox();

	fbo_done(&fbo);
	glPopDebugGroup();
	if (blur_enabled)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Post processing");
		shader_use(&blur);
		glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
		glBindImageTexture(0, fbo.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
		glPushAttrib(GL_TEXTURE_BIT);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, fbo.depth_texture);
		glUniform1i(glGetUniformLocation(blur.program, "horizontal"), 0);
		for (int i = 0; i < blur_passes; i++)
		{
			if (variable_work_size)
				glDispatchComputeGroupSizeARB(1 + width / work_group_size, 1, 1, work_group_size, 1, 1);
			else
				glDispatchCompute(1 + max(width, height) / work_group_size, 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
		glUniform1i(glGetUniformLocation(blur.program, "horizontal"), 1);
		for (int i = 0; i < blur_passes; i++)
		{
			if (variable_work_size)
				glDispatchComputeGroupSizeARB(1 + height / work_group_size, 1, 1, work_group_size, 1, 1);
			else
				glDispatchCompute(1 + height / work_group_size, 1, 1);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
		glPopAttrib();
		glPopDebugGroup();
	}
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Blit");
	glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo.handle);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, fbo.width, fbo.height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glPopDebugGroup();
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
	int64_t lastTime;
	int64_t lastUpdate;
	int64_t frequency;
	QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&lastTime);
	QueryPerformanceCounter((LARGE_INTEGER *)&lastUpdate);
	int nbFrames = 0;
	char fpsString[50];

	SetCapture(hwnd);
	GLuint query;
	GLuint tris = 0;
	glGenQueries(1, &query);
	for(;;)
	{
		// Measure speed
		int64_t currentTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
		nbFrames++;
		if (currentTime - lastTime >= frequency) // If last prinf() was more than 1 sec ago
		{
			// printf and reset timer
			snprintf(fpsString, 50, "%d FPS %f ??/draw (%d triangles)\n", nbFrames, 1000*globals.deltaTime,tris);
			OutputDebugStringA(fpsString);
			nbFrames = 0;
			QueryPerformanceCounter((LARGE_INTEGER *)&lastTime);
			animation_step++;
		}
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           // Is There A Message Waiting?
		{
			TranslateMessage(&msg);             // Translate The Message
			DispatchMessage(&msg);              // Dispatch The Message
		}
		if (msg.message == WM_QUIT)
			break;

		
		globals.time += (double)(currentTime - lastUpdate)/frequency;
		globals.deltaTime = (double)(currentTime - lastUpdate)/frequency;
		QueryPerformanceCounter((LARGE_INTEGER *)&lastUpdate);
		GLuint rdy;
		glGetQueryObjectuiv(query, GL_QUERY_RESULT_AVAILABLE, &rdy);
		if (rdy)
			glGetQueryObjectuiv(query, GL_QUERY_RESULT, &tris);

		glBeginQuery(GL_PRIMITIVES_GENERATED, query);
		render();
		glEndQuery(GL_PRIMITIVES_GENERATED);

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
	case WM_KEYDOWN:
		if (wParam == 'W')
		{
			globals.lookAt.y += 0.1;
			globals.cameraPosition.y += 0.1;
		} else if(wParam == 'S') {
			globals.cameraPosition.y -= 0.1;
			globals.lookAt.y -= 0.1;
		}
		else if (wParam == 'B') {
			blur_enabled = !blur_enabled;
		}
		else if (wParam == 'Q') {
			blur_passes++;
		}
		else if (wParam == 'A') {
			blur_passes--;
		}
		printf("NEW WORK GROUP SIZE: %d\n", work_group_size);
		return 0;
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