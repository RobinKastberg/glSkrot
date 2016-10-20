// Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Project1.h"

#include <windows.h>
#include <GL/GL.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "Dependencies/glew/glew32s.lib")

static HDC hdc;
static HGLRC hglrc;
static HWND hwnd;
static int width;
static int height;
struct shader_program sp;
struct shader_program quadp;
static GLuint m_vaoID[2];
static GLuint m_vboID[3];
static GLuint fbos[2];
static GLuint texs[3];
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
void init_quad()
{
	glBindVertexArray(m_vaoID[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, 80, quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)48);



	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// Two VAOs allocation
	glGenVertexArrays(2, &m_vaoID[0]);
	glGenBuffers(2, &m_vboID[0]);
	// First VAO setup
	glBindVertexArray(m_vaoID[0]);

	glGenTextures(3, texs);

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

	glGenFramebuffers(2, fbos);
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texs[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texs[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texs[2], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

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

	glUseProgram(sp.program);
	cnk = new superchunk();
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 256; j++)
			for (int k = 0; k < 16; k++)
				if ((((float)rand()) / RAND_MAX) > 10*(((float)rand()) / RAND_MAX)*k)
					//if((i-8)*(i-8)+ (j - 8)*(j - 8)+ (k - 8)*(k- 8) < 32)
					cnk->set(i, j, k, rand());
	cnk->update();

	init_quad();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
float x = 0, y = 0, z = 0;
void render()
{
	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();
	
	gluPerspective(90, (float)width / height, 0.01, 1000);
	
	glUniform3f(glGetUniformLocation(quadp.program, "cameraPosition"),50 + 50 * sin(time), 50 + 50 * cos(time), 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(50 + 50 * sin(time), 50 + 50 * cos(time), 10, 100 * cos(0.2*time), 100 * sin(0.2*time), 4, 0, 0, 1);


	
	glBindFramebuffer(GL_FRAMEBUFFER,fbos[0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(sp.program);
	glBindVertexArray(m_vaoID[0]);
	glViewport(0, 0, width, height);
	cnk->render();
	time += 0.01;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glUseProgram(quadp.program);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(quadp.program, "tex"), 0);
	glBindTexture(GL_TEXTURE_2D, texs[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	glUniform1i(glGetUniformLocation(quadp.program, "tex2"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texs[2]);
	glUniform1i(glGetUniformLocation(quadp.program, "tex3"), 2);

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
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		glViewport(0, 0,  width, height);
		glBindTexture(GL_TEXTURE_2D, texs[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, texs[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, texs[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texs[0], 0);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		
		break;
	}
	case WM_SETCURSOR:
		SetCursor(LoadCursor(hInstance, IDC_ARROW));
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}