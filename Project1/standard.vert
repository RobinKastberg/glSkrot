#line 1
layout(location = 0) in vec4 in_Position;

out vec4 out_Position;

void main(){
	out_Position = in_Position;

	gl_Position= projectionMatrix * viewMatrix * out_Position;
}
