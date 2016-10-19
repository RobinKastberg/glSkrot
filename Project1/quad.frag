out vec4 color_out;
in vec2 coords;
in vec3 position;
uniform sampler2D tex;
void main(){
	color_out = texture2D(tex, coords);	
}
