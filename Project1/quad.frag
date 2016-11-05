out vec4 out_Color;

in vec3 v_Normal;
in vec4 v_Position;
in vec2 v_TexCoord;
uniform vec4 ambient = vec4(0.1,0.7,0.2,1);
uniform vec4 diffuse = vec4(0.3,0.3,0,1);

uniform sampler2D tex;
void main() {
	out_Color = ambient + diffuse*clamp(dot(normalize(v_Normal), -vec3(0,0,-1)),0,1)*texture(tex, v_TexCoord).r;
	//out_Color.rgb = (1+normalize(v_Normal))*0.5;
	//out_Color = texture(tex, v_TexCoord).rrrr;
	out_Color.a = 1;
}