#line 1
layout(location = 0) in vec4 in_Position;

out vec4 out_Position;
float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
void main(){
	out_Position = in_Position + vec4(0,0,-deltaTime.x,0);
	float newX  = cameraPosition.x + 5*rand(cameraPosition.xy+vec2(time.x,deltaTime.x)) - 2.5;
	float newY  = cameraPosition.y + 5*rand(cameraPosition.yx+vec2(-time.x,-deltaTime.x)) - 2.5;
	out_Position = mix(out_Position, vec4(newX, newY, cameraPosition.z + 10.0, 1.0), float(out_Position.z < 0.0));
	gl_PointSize = clamp(20/distance(vec3(out_Position),cameraPosition.xyz),0.0,5.0);
	gl_Position= projectionMatrix * viewMatrix * out_Position;
}
