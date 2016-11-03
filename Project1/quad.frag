out vec4 color_out;
in vec4 out_Position;
void main(){
	//color_out.rgba = vec4(1,1,1,1);
	color_out.rgba = 1/vec4(0.1+distance(vec3(out_Position), cameraPosition));
	color_out.a = 1;
	//if(color_out.a < 0.3)
	//	discard;
}