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

	if(stipple && fract(dot(gl_FragCoord.xy, vec2(0.5, 0.5))) < 0.5)
		discard;
	//out_Color = ambient + diffuse*clamp(dot(normalize(v_eyeNormal), -vec3(-1,0,-1)),0,1);
	float attenuation = 2/distance(v_Position.xyz, lightPos.xyz);
	vec3 lightDirection = lightPos.xyz - v_Position.xyz;
	vec3 viewDirection = normalize(lookAt.xyz-cameraPosition.xyz);
	vec3 halfwayDirection = normalize(lightPos.xyz + viewDirection);
float w = pow(1.0 - max(0.0, 
    dot(halfwayDirection, viewDirection)), 5.0);
vec3 specularReflection = attenuation * vec3(_LightColor0) 
    * mix(vec3(_SpecColor), vec3(1.0), w)
    * pow(max(0.0, dot(
    reflect(-lightPos.xyz, v_modelNormal.xyz), 
    viewDirection)), _Shininess);
	out_Color.rgb = attenuation * vec3(_LightColor0) + specularReflection;
	//out_Color.rgb = v_modelNormal;
	//out_Color = texture(tex, v_TexCoord).rrrr;
	out_Color.a = 1;
}