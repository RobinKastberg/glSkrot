#version 330 core
in vec4 pos;
in vec3 modelView_Position;
in vec3 normal;
flat in vec4 texcoord;
uniform int isLight;
layout(location = 0) out vec3 color_out;
layout(location = 1) out vec3 normal_out;
layout(location = 2) out vec3 position_out;
layout(location = 3) out vec4 lighting_out;
const float density = 100;
const float LOG2 = 1.442695;
const vec4 fogColor = vec4(1,1,1,1);
void main(void)
{
vec2 coord33 = gl_PointCoord - gl_FragCoord.xy;  //from [0,1] to [-0.5,0.5]
                 //outside of circle radius?
	if(isLight == 1) {
		lighting_out.rgba = vec4(1,1,0,1);
		return;
	} else {
		lighting_out = vec4(0,0,0,0);
	}
	color_out = vec3(int(texcoord.w) & 1, int(texcoord.w) & 2, int(texcoord.w) & 4);
	//color_out.rgb = texcoord.rgb;
	position_out = pos.xyz;
	//color_out = position_out / 100;
	normal_out = normal;
	
}

