#version 330 compatibility

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.

void main(){
 gl_Position =  gl_ModelViewProjectionMatrix * vec4(vertexPosition_modelspace,1);
}