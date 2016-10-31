#version 330 core
in vec3 pos;
in vec3 modelView_Position;
in vec3 normal;

uniform int isLight;
layout(location = 0) out vec4 color_out;
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


	position_out = pos.xyz;
	color_out.rgb = vec3(1,0,0);
	color_out.rgb = 0.1*mix(terrain_colours[int(pos.z/2)],terrain_colours[int(pos.z/2 )+1],fract(pos.z/2));
	//color_out.rgb = pow(color_out.rgb, vec3(2.2));

	if(dot(color_out.rgb, vec3(0.2126, 0.7152, 0.0722)) > 0.02) {
		lighting_out.rgba = vec4(color_out.rgb,1.0);
	} else {
		lighting_out = vec4(0,0,0,0);
	}
	color_out.a = 1;
	//color_out.rgb = texcoord.rgb;
	
	//color_out = position_out / 100;
	normal_out = normal;
	
}

