// Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Project1.h"

#include <windows.h>
#include <GL/GL.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "Dependencies/glew/glew32s.lib")
#define MRTS 4
static HDC hdc;
static HGLRC hglrc;
static HWND hwnd;
static int width;
static int height;
static int cameraLoc, tex1Loc, tex2Loc, tex3Loc, tex4Loc, shadowLoc, lightLoc;
struct xyz {
	float x;
	float y;
	float z;
} cameraPosition, lookAt, lightPos;
struct shader_program sp;
struct shader_program quadp;
struct shader_program shadowp;
static GLuint m_vaoID[2];
static GLuint m_vboID[3];
static GLuint fbos[2];
static GLuint texs[MRTS];
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

void tick() {
	for (int i = 0; i < 510; i++) {
		for (int j = 0; j < 510; j++) {
			for (int k = 1; k < 32; k++) {
				if (cnk->get(i, j, k) && !cnk->get(i, j, k - 1)) {
					cnk->set(i, j, k - 1, cnk->get(i, j, k));
					cnk->set(i, j, k, 0);
				}
			}
		}
	}
}
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
	glBindTexture(GL_TEXTURE_2D, texs[0]); // Color
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texs[1]); // Normal
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texs[2]); // Position
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texs[3]); // Lighting
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texs[5]); // Shadow map
	glActiveTexture(GL_TEXTURE5);
	cameraLoc = glGetUniformLocation(quadp.program, "cameraPosition");
	lightLoc = glGetUniformLocation(quadp.program, "lightPos");
	tex1Loc = glGetUniformLocation(quadp.program, "tex");
	tex2Loc = glGetUniformLocation(quadp.program, "tex2");
	tex3Loc = glGetUniformLocation(quadp.program, "tex3");
	tex4Loc = glGetUniformLocation(quadp.program, "tex4");
	shadowLoc = glGetUniformLocation(quadp.program, "shadowTex");
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
	//wglSwapIntervalEXT(1);
	glClearColor(0.5, 0.5, 0.5, 1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
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

	glGenTextures(MRTS+2, texs);
	for (int i = 0; i < MRTS; i++) {
		glBindTexture(GL_TEXTURE_2D, texs[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//NULL means reserve texture memory, but texels are undefined
		if(i < MRTS)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		else {
			
		}
	}
	glBindTexture(GL_TEXTURE_2D, texs[MRTS]);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texs[MRTS+1]);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 8192, 8192, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(2, fbos);

	/* OFF SCREEN RENDERING FBO */
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	


	GLenum DrawBuffers[MRTS];
	for (int i = 0; i < MRTS; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, texs[i], 0);
		DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texs[MRTS], 0);

	glDrawBuffers(MRTS-1, DrawBuffers);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);


	/* SHADOW FBO */

	glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texs[MRTS+1], 0);

	glDrawBuffer(GL_NONE);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	shader_init(&sp);
	load_shader_from_resource(&sp, IDR_SHADER1, GL_FRAGMENT_SHADER);
	load_shader_from_resource(&sp, IDR_SHADER2, GL_VERTEX_SHADER);

	shader_init(&quadp);
	load_shader_from_resource(&quadp,IDR_SHADER3, GL_FRAGMENT_SHADER);
	load_shader_from_resource(&quadp,IDR_SHADER4, GL_VERTEX_SHADER);

	shader_init(&shadowp);
	load_shader_from_resource(&shadowp, IDR_SHADER5, GL_FRAGMENT_SHADER);
	load_shader_from_resource(&shadowp, IDR_SHADER6, GL_VERTEX_SHADER);

	glUseProgram(sp.program);
	cnk = new superchunk();
	for (int i = 0; i < 510; i++)
		for (int j = 0; j < 510; j++) {
			cnk->set(i, j, 1, rand());
			int kmax = rand() % 50;
			if (kmax == 36) kmax = 5 + (rand() % 5);
			else kmax = 0;
			for (int k = 0; k < kmax; k++)
			{
				cnk->set(i, j, k, rand());
			}
		}
				/*
				if ((((float)rand()) / RAND_MAX) > 15 * (((float)rand()) / RAND_MAX)*k) {
					//if((i-8)*(i-8)+ (j - 8)*(j - 8)+ (k - 8)*(k- 8) < 32)
					if (rand() % 20 == 0) {
						cnk->set(i, j, k, rand());
						cnk->set(i, j + 1, k, rand());
						cnk->set(i + 1, j + 1, k, rand());
						cnk->set(i + 1, j, k, rand());
					}

					if (rand() % 20 == 3) {
						cnk->set(i, j, k, rand());
						cnk->set(i, j, k+1, rand());
						cnk->set(i, j, k+2, rand());
						cnk->set(i, j, k+3, rand());
					}

					if (rand() % 20 == 5) {
						cnk->set(i, j, k, rand());
						cnk->set(i, j, k + 1, rand());
						cnk->set(i+1, j, k + 1, rand());
						cnk->set(i+2, j, k + 1, rand());
					}

					if (rand() % 20 == 7) {
						cnk->set(i, j, k+1, rand());
						cnk->set(i, j+1, k+1, rand());
						cnk->set(i, j+1, k, rand());
						cnk->set(i, j+2, k + 1, rand());
					}
				} */
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
}
float x = 0, y = 0, z = 0;

void light_camera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50, 50, -50, 50, 0, 1000);
	//gluPerspective(45, 1, 0.1, 800);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(lightPos.x, lightPos.y, 100, cameraPosition.x, cameraPosition.y, 0, 0, 0, 1);
}
void render()
{
	/* SHADOW PASS */

	glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);
	glCullFace(GL_FRONT);
	glViewport(0, 0, 8192, 8192);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shadowp.program);
	light_camera();
	glBindVertexArray(m_vaoID[0]);

	cnk->render();
	glCullFace(GL_BACK);

	/* OFF SCREEN RENDERING PASS */
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (float)width / height, 0.01, 300);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cameraPosition.x = 256 + 256 * sin(0.025*time);
	cameraPosition.y = 256 + 256 * cos(0.025*time);
	cameraPosition.z = 10;

	

	lookAt.x = 256 + 240 * sin(0.1*time);
	lookAt.y = 256 + 240 * cos(0.1*time);
	lookAt.z = 10;
	lightPos.z = 100;
	lightPos.x = lookAt.x;
	lightPos.y = lookAt.y;
	gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z, lookAt.x, lookAt.y, lookAt.z, 0, 0, 1);



	
	glUseProgram(sp.program);

	glBindVertexArray(m_vaoID[0]);
	glViewport(0, 0, width, height);
	glUniform1i(glGetUniformLocation(sp.program, "isLight"), 0);
	float clearValue[] = { 0.0, 0.0, 0.0, 0.0 };
	glClearBufferfv(GL_COLOR, 3, clearValue);
	cnk->render();
	glUniform1i(glGetUniformLocation(sp.program, "isLight"), 1);

	//glPointSize(50);
	//glBegin(GL_POINTS);
	//glVertex3f(lightPos.x, lightPos.y, lightPos.z);
	//glEnd();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(quadp.program);
	light_camera();
	glBindVertexArray(m_vaoID[1]);
	glUniform1i(tex1Loc, 0);
	glUniform1i(tex2Loc, 1);
	glUniform1i(tex3Loc, 2);
	glUniform1i(tex4Loc, 3);
	glUniform1i(shadowLoc, 4);
	glUniform3f(cameraLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform3f(lightLoc, lightPos.x, lightPos.y, lightPos.z);

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
	hwnd = CreateWindowW(wc.lpszClassName, L"jag vet inte vad", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	init();
	ShowWindow(hwnd, nshowcmd);
	UpdateWindow(hwnd);
	unsigned int lastTime = GetTickCount();
	unsigned int lastUpdate = GetTickCount();
	int nbFrames = 0;
	char fpsString[50];
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
			//tick();
		}
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           // Is There A Message Waiting?
		{
			TranslateMessage(&msg);             // Translate The Message
			DispatchMessage(&msg);              // Dispatch The Message
		}
		if (msg.message == WM_QUIT)
			break;

		
		time += 0.001f*(currentTime - lastUpdate);
		lastUpdate = GetTickCount();
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
		glActiveTexture(GL_TEXTURE6);
		for (int i = 0; i < MRTS; i++) {
			glBindTexture(GL_TEXTURE_2D, texs[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}
		glBindTexture(GL_TEXTURE_2D, texs[MRTS]);
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