#line 1
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

out vec4 v_Position;
out vec3 v_eyeNormal;
out vec3 v_modelNormal;
out vec2 v_TexCoord;


void main(){
	mat4 modelMatrix = instance[currentModel].modelMatrix;
	mat3 normalMatrix = inverse(transpose(mat3(viewMatrix * modelMatrix)));
	v_Position = in_Position;
	v_TexCoord = in_Position.xy;
	v_modelNormal = in_Normal;
	v_eyeNormal = normalMatrix * normalize(in_Normal);
	gl_Position= projectionMatrix * viewMatrix * modelMatrix * v_Position;
}
