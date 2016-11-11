#include "stdafx.h"

#include "shaders.h"

const char * const INCLUDE =
"#version 330 compatibility\n"
"#define debug(on)\n"
"layout(std140) uniform global {"
"	mat4 viewMatrix;"
"	mat4 projectionMatrix;"
"	mat4 lightMatrix;"
"	vec4 cameraPosition;"
"	vec4 lookAt;"
"	vec4 lightPos;"
"	vec4 time;"
"	vec4 deltaTime;"
"};\n"
"layout(std140) uniform model {"
"	mat4 modelMatrix[128];"
"};\n"
"uniform int currentModel;\n";

void shader_init(struct shader_program * self, const char * name)
{
	self->program = glCreateProgram();
	self->name = name;
	NAME(GL_PROGRAM, self->program, self->name);
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
		assert(infoLog);
		getInfo(shader, status, &status, infoLog);
		OutputDebugStringA(infoLog);
		exit(0);
		free(infoLog);
		//We don't need the shader anymore.
		glDeleteShader(shader);
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
bool shader_source(struct shader_program *self, GLenum type, const unsigned char * str, int size)
{
	GLuint shader = glCreateShader(type);
	int sz[2] = { strlen((const char *)INCLUDE), size };
	char *ptrs[2] = { (char *)INCLUDE, (char *)str };
	glShaderSource(shader, 2,  (char **)ptrs, sz);
	glCompileShader(shader);
	check_compile(shader, GL_COMPILE_STATUS);
	glAttachShader(self->program, shader);
	if (type == GL_FRAGMENT_SHADER)
	{
		OutputDebugStringA("FRAG");
		self->frag_shader = shader;
		NAME(GL_FRAGMENT_SHADER, shader, self->name);
	}
	else if (type == GL_VERTEX_SHADER) {
		OutputDebugStringA("VERT");
		self->vert_shader = shader;
		NAME(GL_VERTEX_SHADER, shader, self->name);
	}
	else if (type == GL_TESS_CONTROL_SHADER) {
		OutputDebugStringA("TESC");
		self->tesc_shader = shader;
		NAME(GL_TESS_CONTROL_SHADER, shader, self->name);
	}
	else if (type == GL_TESS_EVALUATION_SHADER) {
		OutputDebugStringA("TESE");
		self->tese_shader = shader;
		NAME(GL_TESS_EVALUATION_SHADER, shader, self->name);
	}

	if (self->vert_shader && self->frag_shader)
	{
		OutputDebugStringA("LINKING...");
		glLinkProgram(self->program);
		check_compile(self->program, GL_LINK_STATUS);
		glUniformBlockBinding(self->program, glGetUniformBlockIndex(self->program, "global"), 0);
		glUniformBlockBinding(self->program, glGetUniformBlockIndex(self->program, "model"), 1);
	}
	return true;
}
