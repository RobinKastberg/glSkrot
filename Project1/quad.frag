out vec4 out_Color;

in vec2 v_TexCoord;

uniform sampler2D tex;


void main(){
	out_Color = clamp(texture(tex, v_TexCoord).r*vec4(1,0,0,1),0,1);
	out_Color.a = 1;
}