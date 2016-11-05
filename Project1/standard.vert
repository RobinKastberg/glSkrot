#line 1
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

out vec4 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;


void main(){
	mat3 normalMatrix = inverse(transpose(mat3(viewMatrix)));
	v_Position = in_Position;
	v_TexCoord = in_Position.xy;
	v_Normal = normalMatrix * normalize(in_Normal);
	v_Position.xyz = 10*v_Position.xyz;
	gl_Position= projectionMatrix * viewMatrix * v_Position;
		

}
