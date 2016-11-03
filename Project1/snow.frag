#line 1
out vec4 color_out;
in vec4 out_Position;
void main(){
	//color_out.rgba = vec4(1,1,1,1);
	color_out.rgba = 2/vec4(distance(vec3(out_Position), cameraPosition.xyz));
	color_out.rgba = color_out * (1 - 1.5*distance(gl_PointCoord, vec2(0.5, 0.5)));
	//color_out.a = 1;
	if(color_out.a < 0.3)
		discard;
}