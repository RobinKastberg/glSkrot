#version 330 compatibility

layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;

out vec4 pos;
out vec3 normal;
void main(void)
{
	vec4 tmp = vec4(in_Position.xyz, 1.0);
	//tmp.xyz += 0.3*normalize(in_Normal);
	gl_Position = gl_ModelViewProjectionMatrix*tmp;
	pos = tmp;
	normal = gl_NormalMatrix * in_Normal;
}

