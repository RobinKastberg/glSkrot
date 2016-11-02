layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 dxyz;

out vec3 pos;
out vec3 normal;

uniform float time;


void main(void)
{
	vec4 tmp = in_Position;
	pos = in_Position.xyz + dxyz;
	//tmp.xyz += 0.3*normalize(in_Normal);
	//pos.x = (50-pos.z)*cos(2*3.14*pos.x/256);
	//pos.z = (50-pos.z)*sin(2*3.14*pos.x/256);
	//pos.y = 50*pos.y/256;
	gl_Position = projectionMatrix * viewMatrix * vec4(pos,1);
	


	normal = vec3(viewMatrix * vec4(in_Normal,0.0));;
}

