#version 330 core

layout(std140) uniform global {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 lightMatrix;
	vec3 cameraPosition;
	vec3 lookAt;
	vec3 lightPos;
};

void main(){
}