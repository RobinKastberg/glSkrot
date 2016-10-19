// Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Project1.h"

#include <windows.h>
#include <GL/GL.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "Dependencies/glew/glew32s.lib")

static HDC hdc;
static HGLRC hglrc;
static HWND hwnd;
static struct shader_program sp;
static GLuint m_vaoID[2];
static GLuint m_vboID[3];
static GLuint fbos[2];
static GLuint texs[2];
static chunk *cnk;
float quad[] =  {  -1.0f, -1.0f, 0.0f,
1.0f, -1.0f, 0.0f,
1.0f, 1.0f, 0.0f,
-1.0f, 1.0f, 0.0f,

0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
0.0f, 1.0f
};
void init_quad()
{
	glBindVertexArray(m_vaoID[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, 80, quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)48);



	glBindTexture(GL_TEXTURE_2D, 0);
}
void draw_quad()
{
	glBindVertexArray(m_vaoID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);

	glBindTexture(GL_TEXTURE_2D, texs[0]);
	glDrawArrays(GL_QUADS, 0, 4);
}
HINSTANCE hInstance;
float time = 0;
void load_shader_from_resource(int res, GLenum type)
{
	HRSRC hRes = FindResourceA(hInstance, MAKEINTRESOURCEA(res), "SHADER");
	HGLOBAL hData = LoadResource(hInstance, hRes);
	LPVOID frag = LockResource(hData);
	OutputDebugStringA((char *)frag);
	OutputDebugStringA("WAT");
	shader_source(&sp, type, (char *)frag, SizeofResource(hInstance, hRes));
	UnlockResource(hData);
	FreeResource(hRes);
}
void init()
{
	wglSwapIntervalEXT(1);

	glClearColor(1, 1, 0, 1);
	glViewport(0, 0, 640, 480);
	// Two VAOs allocation
	glGenVertexArrays(2, &m_vaoID[0]);

	// First VAO setup
	glBindVertexArray(m_vaoID[0]);

	glGenTextures(1, texs);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenFramebuffers(2, fbos);
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texs[0], 0);
	if (glCheckFramebufferStatus(fbos[0]) != GL_FRAMEBUFFER_COMPLETE)
	{
		OutputDebugStringA("BAD BAD FRAMEBUFFER\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	shader_init(&sp);
	load_shader_from_resource(IDR_SHADER1, GL_FRAGMENT_SHADER);
	load_shader_from_resource(IDR_SHADER2, GL_VERTEX_SHADER);
	glUseProgram(sp.program);
	cnk = new chunk();
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			for (int k = 0; k < 16; k++)
				if ((((float)rand()) / RAND_MAX) > 0.9)
				//	if((i-8)*(i-8)+ (j - 8)*(j - 8)+ (k - 8)*(k- 8) < 32)
					cnk->set(i, j, k, 200);
	cnk->update();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
float x = 0, y = 0, z = 0;
void render()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform1f(glGetUniformLocation(sp.program, "rotate"), 0.01*time);
	glUniform1f(glGetUniformLocation(sp.program, "time"), time);
	glBindVertexArray(m_vaoID[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	glUseProgram(sp.program);
	cnk->render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	draw_quad();
	time += 0.001;
// select first VAO
	//glBindVertexArray(0);
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
			0
		};
		
		hglrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hglrc);
		glewInit();
		hglrc = wglCreateContextAttribsARB(hdc, 0, attribs);

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
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));


		break;
	}
	case WM_SETCURSOR:
		SetCursor(LoadCursor(hInstance, IDC_ARROW));
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}