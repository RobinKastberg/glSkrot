out vec4 color_out;
in vec2 coords;
in vec3 position;
uniform sampler2D tex;
void main(){
	vec2 coord = coords;
	//coord.x += position.x*3.5;
	//coord.y += position.y*3.5;
	color_out = texture2D(tex, coord);	
	color_out.a = 1;
}
