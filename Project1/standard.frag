layout(location = 0) out vec4 out_Color;

in vec3 v_modelNormal;
flat in vec3 v_faceNormal;
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
	out_Color.rgb = normalize(v_modelNormal.xyz);
	out_Color.a = 1;
}