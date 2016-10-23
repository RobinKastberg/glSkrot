#version 330 core
in vec4 pos;
in vec3 modelView_Position;
in vec3 modelView_Normal;
flat in vec4 texcoord;
layout(location = 0) out vec3 color_out;
layout(location = 1) out vec3 normal_out;
layout(location = 2) out vec3 position_out;

const float density = 100;
const float LOG2 = 1.442695;
const vec4 fogColor = vec4(1,1,1,1);
void main(void)
{
	color_out = vec4(1,1,1,1);
	return;
	color_out = vec3(texcoord.w == 100 ? 1 : 1, 1,0);
	//color_out.rgb = texcoord.rgb;
	position_out = modelView_Position;
	//color_out = position_out / 100;
	normal_out = modelView_Normal;
}

