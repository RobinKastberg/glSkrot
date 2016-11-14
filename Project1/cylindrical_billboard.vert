#line 1
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

out vec4 v_Position;
out vec3 v_eyeNormal;
out vec3 v_modelNormal;
out vec2 v_TexCoord;


void main(){
	mat4 worldMatrix = modelMatrix[currentModel];
	mat4 vMatrix = viewMatrix;
	mat4 rotate;
	mat3 inv = inverse(mat3(viewMatrix));
	rotate[0].xyz = inv[0].xyz;
	rotate[1].xyz = inv[1].xyz;
	rotate[2].xyz = vec3(0,0,1); //inv[2].xyz;
	rotate[3].w = 1.0;
	v_Position = rotate * in_Position;
	mat4 viewModelMatrix =  viewMatrix * worldMatrix ;

	mat3 normalMatrix = inverse(transpose(mat3(viewModelMatrix)));
	//v_Position = in_Position;
	v_TexCoord = in_Position.xy;
	v_modelNormal = in_Normal;
	v_eyeNormal = normalMatrix * normalize(in_Normal);
	gl_Position=  projectionMatrix * viewMatrix * worldMatrix * v_Position;
}
