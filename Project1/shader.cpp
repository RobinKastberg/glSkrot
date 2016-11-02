#include "stdafx.h"

const char *INCLUDE =
"#version 330 compatibility\n"
"#define debug(on)\n";

void shader_init(struct shader_program *self)
{
	self->program = glCreateProgram();
}
static bool check_compile(GLuint shader, GLenum type)
{
	int status;
	void(__stdcall *get)(GLuint shader, GLenum pname, GLint *params);
	void(__stdcall *getInfo)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
	if (type == GL_LINK_STATUS)
	{
		//get = (PFNGLGETSHADERIVPROC)
		get = glGetProgramiv;
		getInfo = glGetProgramInfoLog;
	} else {
		get = glGetShaderiv;
		getInfo = glGetShaderInfoLog;
	}
	get(shader, type, &status);
	if (status == GL_FALSE) {
		get(shader, GL_INFO_LOG_LENGTH, &status);
		//The maxLength includes the NULL character
		char *infoLog = (char *)malloc(status);
		getInfo(shader, status, &status, infoLog);
		CRASH(infoLog);
		free(infoLog);
		//We don't need the shader anymore.
		glDeleteShader(shader);
	}
	else {

		if (type == GL_LINK_STATUS && GLEW_VERSION_4_1) {
			glGetProgramiv(shader, GL_PROGRAM_BINARY_LENGTH, &status);
			char *infoLog = (char *)malloc(status);
			GLenum bin;
			glGetProgramBinary(shader, status, &status, &bin, infoLog);
			FILE *f;
			volatile int err = fopen_s(&f, "shader.bin", "wb");
			fwrite(infoLog, 1, status, f);
			fclose(f);
			free(infoLog);
		}
		
	}
	return false;
}
void shader_verify(const struct shader_program *self)
{
	char buf[100];
	int ret;

	glValidateProgram(self->program);
	glGetProgramiv(self->program, GL_VALIDATE_STATUS, &ret);
	if (ret == GL_FALSE) {
		glGetProgramInfoLog(self->program, 100, 0, buf);
		OutputDebugStringA(buf);
		exit(1);
	}
}
bool shader_source(struct shader_program *self, GLenum type, const char *  str, int size)
{
	GLuint shader = glCreateShader(type);
	int sz[2] = { strlen(INCLUDE), size };
	const char *  ptrs[2] = { INCLUDE, str };
	glShaderSource(shader, 2,  (char **)ptrs, sz);
	glCompileShader(shader);
	check_compile(shader, GL_COMPILE_STATUS);
	glAttachShader(self->program, shader);
	if (type == GL_FRAGMENT_SHADER)
	{
		OutputDebugStringA("FRAG");
		self->frag_shader = shader;
	}
	else if (type == GL_VERTEX_SHADER) {
		OutputDebugStringA("VERT");
		self->vert_shader = shader;
	}

	if (self->vert_shader && self->frag_shader)
	{
		OutputDebugStringA("LINKING...");
		glLinkProgram(self->program);
		check_compile(self->program, GL_LINK_STATUS);
	}
	return true;
}
