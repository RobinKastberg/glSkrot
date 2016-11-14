#include "stdafx.h"
void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{

	printf("(%d):%s\n", id, message);
}

void debug_init() 
{
	if (GLEW_VERSION_4_3 || GLEW_KHR_debug) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		GLuint ignore = 131185;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
		glDebugMessageControl(GL_DEBUG_SOURCE_API,
			GL_DEBUG_TYPE_OTHER,
			GL_DONT_CARE,
			1,
			&ignore,
			false);
		glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION,
			GL_DEBUG_TYPE_PUSH_GROUP,
			GL_DONT_CARE,
			1,
			&unusedIds,
			false);
		glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION,
			GL_DEBUG_TYPE_POP_GROUP,
			GL_DONT_CARE,
			1,
			&unusedIds,
			false);
	}
}