#include "stdafx.h"

#include "shaders.h"

const char * const INCLUDE =
"#version 430 compatibility\n"
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
"uniform int currentModel;\n"
"#line 1\n";
static object *shaderCache = NULL;
static GLuint currentProgram;
void shader_init(struct shader_program * self, const char * name)
{
	if (shaderCache == NULL)
	{
		shaderCache = (object *)malloc(sizeof(object));
		object_new(shaderCache, "shaderCache");
	}
	struct shader_program *alreadySelf = (shader_program *)object_get_int(shaderCache, name);
	if (alreadySelf) {
		*self = *alreadySelf;
		return;
	}
	self->program = glCreateProgram();
	self->frag_shader = 0;
	self->vert_shader = 0;
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
	if ((self->vert_shader && type == GL_VERTEX_SHADER)
		|| (self->frag_shader && type == GL_FRAGMENT_SHADER)
		|| (self->tesc_shader && type == GL_TESS_CONTROL_SHADER)
		|| (self->tese_shader && type == GL_TESS_EVALUATION_SHADER))
	{
		OutputDebugStringA("Changing attached shader not supported");
		return false;
	}
	GLuint shader = glCreateShader(type);
	int sz[2] = { strlen((const char *)INCLUDE), size };
	char *ptrs[2] = { (char *)INCLUDE, (char *)str };
	glShaderSource(shader, 2,  (char **)ptrs, sz);
	glCompileShader(shader);
	check_compile(shader, GL_COMPILE_STATUS);
	glAttachShader(self->program, shader);
	OutputDebugStringA(self->name);
	if (type == GL_FRAGMENT_SHADER && !self->frag_shader)
	{
		OutputDebugStringA("FRAG\n");
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
		glUseProgram(self->program);
		glUniformBlockBinding(self->program, glGetUniformBlockIndex(self->program, "global"), 0);
		glUniformBlockBinding(self->program, glGetUniformBlockIndex(self->program, "model"), 1);

		glUniform1i(glGetUniformLocation(self->program, "texture0"), 0);
		glUniform1i(glGetUniformLocation(self->program, "texture1"), 1);
		glUniform1i(glGetUniformLocation(self->program, "texture2"), 2);
		object_set_int(shaderCache, self->name, (int)self);
	}
	return true;
}
bool shader_compute(struct shader_program *self, const unsigned char * str, int size)
{
	if (self->comp_shader)
	{
		OutputDebugStringA("Changing attached shader not supported");
		return false;
	}
	shader_source(self, GL_COMPUTE_SHADER, str, size);
	OutputDebugStringA("LINKING...");
	glLinkProgram(self->program);
	check_compile(self->program, GL_LINK_STATUS);
	glUseProgram(self->program);
	glUniformBlockBinding(self->program, glGetUniformBlockIndex(self->program, "global"), 0);
	glUniformBlockBinding(self->program, glGetUniformBlockIndex(self->program, "model"), 1);

	glUniform1i(glGetUniformLocation(self->program, "texture0"), 0);
	glUniform1i(glGetUniformLocation(self->program, "texture1"), 1);
	glUniform1i(glGetUniformLocation(self->program, "texture2"), 2);
	object_set_int(shaderCache, self->name, (int)self);
	return true;
}
void shader_use(const struct shader_program *self)
{
	if (currentProgram != self->program)
		glUseProgram(self->program);
	currentProgram = self->program;
}
