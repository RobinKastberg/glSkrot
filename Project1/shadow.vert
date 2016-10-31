#version 330 core

layout(std140) uniform global {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 lightMatrix;
	vec3 cameraPosition;
	vec3 lookAt;
	vec3 lightPos;
};

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.

void main(){
 gl_Position =  lightMatrix * vec4(vertexPosition_modelspace,1);
}