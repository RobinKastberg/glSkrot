#version 330 core
in vec4 pos;
out vec4 color_out;
varying vec4 texcoord;
void main(void)
{
	color_out = vec4(texcoord.w / 128.0, texcoord.w / 256.0, texcoord.w / 512.0, 1.0);
}

