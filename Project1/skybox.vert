#line 1
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

out vec4 v_Position;
out vec2 v_TexCoord;
out vec3 eyeDirection;
uniform sampler2D tex;

void main(){
	v_Position = in_Position;
	v_Position.z = 0.9999999;
	v_TexCoord = in_TexCoord;
	gl_Position= v_Position;

	mat4 inverseProjection = inverse(projectionMatrix);
	mat4 inverseView = inverse(viewMatrix);
	vec3 unprojected = (inverseProjection * v_Position).xyz;
	eyeDirection = (inverseView * vec4(unprojected, 0.0)).xyz;
}
