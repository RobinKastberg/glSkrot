#version 330
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 coord;
out vec2 coords;
out vec3 position;
uniform float time;
void main(){
coords=coord.st;
position = pos;
gl_Position=vec4(pos, 1.0);
vec4 tmp = gl_Position;
gl_Position.x = tmp.x*cos(time) - tmp.y*sin(time);
gl_Position.y = tmp.x*sin(time) + tmp.y*cos(time);
gl_Position.xy *= sin(time);
gl_Position.x += sin(time);
}
