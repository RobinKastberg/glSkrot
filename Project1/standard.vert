#line 1
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

out vec4 v_Position;
out vec2 v_TexCoord;

uniform sampler2D tex;

void main(){
	v_Position = in_Position;
	v_Position.xyz *= 0.1;
	v_TexCoord = in_TexCoord;
	v_Position += texture2DLod(tex, v_TexCoord, 0.0);
	gl_Position= projectionMatrix * viewMatrix * v_Position;
}
