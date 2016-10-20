#version 330 compatibility

layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec4 in_Normal;
out vec4 pos;
varying vec3 modelView_Position;
out vec3 modelView_Normal;
uniform float time;
uniform float rotate;
uniform float dx, dy, dz;
mat4 projection = mat4(1, 0, 0, 0, 0, 16.0/9,0,0, 0, 0, 0.999999, 1, 0, 0, -0.1, 0);
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}
varying vec4 texcoord;
void main(void)
{
	texcoord = in_Position;
	vec4 tmp = vec4(in_Position.xyz, 1.0);
	modelView_Position = (in_Position + vec3(dx, dy, dz)).xyz;
	modelView_Normal = gl_NormalMatrix * vec3(in_Normal);
	gl_Position = gl_ModelViewProjectionMatrix*tmp;
}

