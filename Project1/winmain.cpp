// Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Project1.h"

#include <windows.h>
#include <GL/GL.h>
#include "model.h"
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "Dependencies/glew/glew32s.lib")

struct model *cube;
static HDC hdc;
static HGLRC hglrc;
static HWND hwnd;
static int width;
static int height;
static int cameraLoc, tex1Loc, tex2Loc, tex3Loc;
struct shader_program sp;
struct shader_program quadp;
static GLuint m_vaoID[2];
static GLuint m_vboID[3];
static GLuint fbos[2];
static GLuint texs[4];
static superchunk *cnk;
__declspec(align(16)) static float quad[] =  {  -1.0f, -1.0f, 0.0f,
1.0f, -1.0f, 0.0f,
1.0f, 1.0f, 0.0f,
-1.0f, 1.0f, 0.0f,

0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
0.0f, 1.0f
};
void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {

	OutputDebugStringA(message);
}
inline void NAME(GLenum type, GLuint id, const char * const name)
{
	if (glewGetExtension("GL_KHR_debug")) {
		glObjectLabel(type, id, strlen(name), name);
	}
}
void init_quad()
{
	glBindVertexArray(m_vaoID[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, 80, quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)48);





	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texs[2]);
	glActiveTexture(GL_TEXTURE4);

	cameraLoc = glGetUniformLocation(quadp.program, "cameraPosition");
	tex1Loc = glGetUniformLocation(quadp.program, "tex");
	tex2Loc = glGetUniformLocation(quadp.program, "tex2");
	tex3Loc = glGetUniformLocation(quadp.program, "tex3");
}
void draw_quad()
{
	glBindVertexArray(m_vaoID[1]);
	glDrawArrays(GL_QUADS, 0, 4);

}
HINSTANCE hInstance;
float time = 0;
void load_shader_from_resource(struct shader_program *p, int res, GLenum type)
{
	HRSRC hRes = FindResourceA(hInstance, MAKEINTRESOURCEA(res), "SHADER");
	HGLOBAL hData = LoadResource(hInstance, hRes);
	LPVOID frag = LockResource(hData);
	//OutputDebugStringA((char *)frag);
	OutputDebugStringA("WAT");
	shader_source(p, type, (char *)frag, SizeofResource(hInstance, hRes));
	UnlockResource(hData);
	FreeResource(hRes);
}
void init()
{
	wglSwapIntervalEXT(1);
	glClearColor(1, 1, 0, 1);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	if (glewGetExtension("GL_KHR_debug")) {
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
		glDebugMessageControl(GL_DONT_CARE,
			GL_DEBUG_TYPE_MARKER,
			GL_DONT_CARE,
			0,
			&unusedIds,
			false);
	}
	// Two VAOs allocation
	glGenVertexArrays(2, &m_vaoID[0]);
	glGenBuffers(2, &m_vboID[0]);
	// First VAO setup
	glBindVertexArray(m_vaoID[0]);

	glGenTextures(4, texs);

	glBindTexture(GL_TEXTURE_2D, texs[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texs[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texs[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texs[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(2, fbos);
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texs[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texs[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texs[2], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texs[3], 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffers);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	shader_init(&sp);
	load_shader_from_resource(&sp, IDR_SHADER1, GL_FRAGMENT_SHADER);
	load_shader_from_resource(&sp, IDR_SHADER2, GL_VERTEX_SHADER);

	shader_init(&quadp);
	load_shader_from_resource(&quadp,IDR_SHADER3, GL_FRAGMENT_SHADER);
	load_shader_from_resource(&quadp,IDR_SHADER4, GL_VERTEX_SHADER);

	cnk = new superchunk();
	for (int i = 1; i < 15; i++)
		for (int j = 1; j < 15; j++)
			for (int k = 1; k < 15; k++)
				//if ((((float)rand()) / RAND_MAX) > 10*(((float)rand()) / RAND_MAX)*k)
					//if((i-8)*(i-8)+ (j - 8)*(j - 8)+ (k - 8)*(k- 8) < 32)
				if(k == 2)
					cnk->set(i, j, k, 1);
	//cnk->set(1, 1, 1, 1);
	//cnk->set(2, 1, 1, 1);
	//cnk->set(2, 2, 1, 1);
	//cnk->set(1, 2, 1, 1);
	cnk->update();
	init_quad();

	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	NAME(GL_TEXTURE, texs[0], "Color texture");
	NAME(GL_TEXTURE, texs[1], "Normal texture");
	NAME(GL_TEXTURE, texs[2], "Position texture");
	NAME(GL_PROGRAM, sp.program, "main");
	NAME(GL_SHADER, sp.vert_shader, "main");
	NAME(GL_SHADER, sp.frag_shader, "main");
	NAME(GL_PROGRAM, quadp.program, "quad");
	NAME(GL_SHADER, quadp.vert_shader, "quad");
	NAME(GL_SHADER, quadp.frag_shader, "quad");
	NAME(GL_FRAMEBUFFER, fbos[0], "Deferred Rendering");

	//cube = make_cube();
}
float x = 0, y = 0, z = 0;

void render()
{
	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();
	
	gluPerspective(90, (float)width / height, 0.01, 100);
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(50 + 50 * sin(time), 50 + 50 * cos(time), 10, 100 * cos(0.2*time), 100 * sin(0.2*time), 4, 0, 0, 1);
	gluLookAt(0 + 5 * sin(time), 0 + 5 * cos(time), 5, 7, 7, 2, 0, 0, 1);

	glBindFramebuffer(GL_FRAMEBUFFER,fbos[0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(sp.program);
	glBindVertexArray(m_vaoID[0]);
	glViewport(0, 0, width, height);

	shader_verify(&sp);
	//glUseProgram(0);
	glUniform1f(glGetUniformLocation(sp.program, "time"), time);

	cnk->render();
	//draw(cube);

	time += 0.01;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glUseProgram(quadp.program);
	glBindVertexArray(m_vaoID[1]);
	glUniform1i(tex1Loc, 0);
	glUniform1i(tex2Loc, 1);
	glUniform1i(tex3Loc, 2);
	glUniform3f(cameraLoc, 50 + 50 * sin(time), 50 + 50 * cos(time), 10);

	shader_verify(&quadp);
	draw_quad();
}
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int nshowcmd)
{
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"oglversionchecksample";
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wc.style = CS_OWNDC;
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
	}
	hwnd = CreateWindowW(wc.lpszClassName, L"openglversioncheck", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	init();
	ShowWindow(hwnd, nshowcmd);
	UpdateWindow(hwnd);
	for(;;)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           // Is There A Message Waiting?
		{
			TranslateMessage(&msg);             // Translate The Message
			DispatchMessage(&msg);              // Dispatch The Message
		}
		if (msg.message == WM_QUIT)
			break;

		render();
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			OutputDebugStringA((char *)gluErrorString(err));
		}
		SwapBuffers(hdc);
	}
	return msg.wParam;
}

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
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
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
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texs[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, texs[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, texs[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, texs[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		
		break;
	}
	case WM_SETCURSOR:
		SetCursor(LoadCursor(hInstance, IDC_ARROW));
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}