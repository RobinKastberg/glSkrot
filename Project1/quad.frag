#version 330
out vec4 color_out;
in vec2 coords;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform vec3 cameraPosition;
void main(){
	vec3 image = texture2D(tex, coords).rgb;
	vec3 normal  = vec3(texture2D(tex2, coords)).rgb;
	vec3 position  = texture2D(tex3, coords).rgb;
    vec3 light = vec3(150,150,200);
    vec3 lightDir = light - position.xyz ;
    
    normal = normalize(normal);
    lightDir = normalize(lightDir);
    
    vec3 eyeDir = normalize(cameraPosition-position.xyz);
    vec3 vHalfVector = normalize(reflect(vec3(0,0,1), normal));
    

	color_out.rgb = max(dot(normal,lightDir),0) * image +
                      pow(max(dot(eyeDir,vHalfVector),0.0),64);
	color_out.a = 1;
}
