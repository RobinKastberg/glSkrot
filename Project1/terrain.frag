out vec4 out_Color;

in vec3 v_modelNormal;
in vec3 v_eyeNormal;
in vec4 v_Position;
in vec2 v_TexCoord;
uniform vec4 ambient = vec4(0.1,0.5,0.2,1);
uniform vec4 diffuse = vec4(0.3,0.3,0.3,1);
uniform bool stipple = false;
uniform sampler2D tex;
const float _Shininess = 0.5;
const vec3 _SpecColor = vec3(1,1,1);
const vec3 _LightColor0 = vec3(1,1,0);
void main() {


	//out_Color.rgb = ambient.xyz / distance(lightPos.xyz, v_Position.xyz);
	out_Color.rgb = ambient.rgb + diffuse.rgb*clamp(dot(v_eyeNormal, vec3(0,0,-1)),0,1);
	//out_Color = texture(tex, v_TexCoord).rrrr;
	out_Color.a = 1;
}