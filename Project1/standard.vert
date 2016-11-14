#line 1
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

out vec4 v_Position;
out vec3 v_modelNormal;
flat out vec3 v_faceNormal;
out vec2 v_TexCoord;


void main(){
	mat4 worldMatrix = modelMatrix[currentModel];
	mat3 normalMatrix = inverse(transpose(mat3(worldMatrix)));
	v_Position = worldMatrix * in_Position;
	v_TexCoord = in_Position.xy;
	v_modelNormal = normalMatrix * in_Normal;
	v_faceNormal = normalMatrix * in_Normal;
	gl_Position=  projectionMatrix * viewMatrix * worldMatrix * v_Position;
}
