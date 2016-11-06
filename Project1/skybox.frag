out vec4 out_Color;

in vec2 v_TexCoord;
in vec3 eyeDirection;
uniform samplerCube tex;


void main(){
	vec4 ambient = vec4(158.0/255.0, 179.0/255.0, 221.0/255.0, 1.0);
	vec4 gradient = mix(vec4(6.0/255.0, 36.0/255.0, 80.0/255.0, 1.0),
					    vec4(132.0/255.0, 139.0/255.0, 255.0/255.0, 1.0),
						0.5*(-eyeDirection.z+1)
						);
	float newZ = 3*eyeDirection.z;	
	out_Color = (ambient + gradient)/2.0;
	vec3 eye = eyeDirection;
	eye.x = cos(time.x)*eyeDirection.x + sin(time.x)*eyeDirection.z;
	eye.z = -sin(time.x)*eyeDirection.x+ cos(time.x)*eyeDirection.z;
	if(texture(tex, eye).r > 0.5)
		out_Color = mix((ambient + gradient)/2.0, vec4(1,1,1,1), texture(tex, eye).r-0.5);
	out_Color.a = 1;
}