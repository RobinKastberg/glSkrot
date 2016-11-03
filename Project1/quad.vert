layout(location = 0) in vec4 in_Position;

out vec4 out_Position;

uniform float time;
layout(std140) uniform particles {
	float scale;
	vec4 positions[100];
	
};
void main(){
	out_Position = in_Position;
	gl_PointSize = clamp(5/distance(vec3(out_Position),cameraPosition),0.0,2.0);
	gl_Position= projectionMatrix * viewMatrix * out_Position;
}
