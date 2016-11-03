#line 1
layout(location = 0) in vec4 in_Position;

out vec4 out_Position;

void main(){
	out_Position = in_Position + vec4(0,0,-deltaTime.x,0);
	out_Position.z = mod(out_Position.z, 10.0);
	gl_PointSize = clamp(5/distance(vec3(out_Position),cameraPosition.xyz),0.0,2.0);
	gl_Position= projectionMatrix * viewMatrix * out_Position;
}
