#version 330 core
in vec4 pos;
in vec3 modelView_Position;
in vec3 normal;

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

	vec3 terrain_colours[5];
    terrain_colours[0] = vec3(0.0,0.0,0.6);
	terrain_colours[1] = vec3(0.0,0.0,0.6);
    terrain_colours[2] = vec3(0.1, 0.3, 0.1);
    terrain_colours[3] =  vec3(0.4, 0.8, 0.4);
    terrain_colours[4] = vec3(1.0,1.0,1.0);

	if(isLight == 1) {
		lighting_out.rgba = vec4(1,1,0,1);
		return;
	} else {
		lighting_out = vec4(0,0,0,0);
	}
	color_out = vec3(1,0,0);
	color_out = mix(terrain_colours[int(pos.z)],terrain_colours[int(pos.z)+1],fract(pos.z));
	//color_out.rgb = texcoord.rgb;
	position_out = pos.xyz;
	//color_out = position_out / 100;
	normal_out = normal;
	
}

