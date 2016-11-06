out vec4 out_Color;

in vec3 v_modelNormal;
in vec3 v_eyeNormal;
in vec4 v_Position;
in vec2 v_TexCoord;
uniform vec4 ambient = vec4(0.1,0.5,0.2,1);
uniform vec4 diffuse = vec4(0.3,0.3,0.3,1);
uniform bool stipple = false;
uniform sampler2D tex;
void main() {
	if(stipple && fract(dot(gl_FragCoord.xy, vec2(0.5, 0.5))) < 0.5)
		discard;
	out_Color = ambient + diffuse*clamp(dot(normalize(v_eyeNormal), -vec3(-1,0,-1)),0,1);
	//out_Color.rgb = normalize(v_eyeNormal);
	//out_Color = texture(tex, v_TexCoord).rrrr;
	out_Color.a = 1;
}