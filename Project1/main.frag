#version 330 core

layout(location = 0) out vec3 color_out;
layout(location = 1) out vec3 normal_out;
in vec4 pos;
in vec3 normal;
void main(void)
{
	color_out = vec3(1,0,0)*(0.5*pos.x+0.5) + vec3(0,1,0)*(0.5*pos.y+0.5) + vec3(0,0,1)*(0.5*pos.z+0.5);
	normal_out = 0.5*normalize(normal)+0.5;
}

