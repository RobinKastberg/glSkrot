#version 330
#extension GL_ARB_texture_gather : enable
#ifdef GL_ARB_texture_gather
#define tex textureOffset
#else
#define tex textureOffset
#endif
out vec4 color_out;
in vec2 coords;
uniform sampler2D pprocess_in; 
const float[9] weights = float[](0.00013383022576490326, 0.004431848411938341, 0.05399096651318985, 0.2419707245191454, 0.3989422804014327, 0.2419707245191454, 0.05399096651318985, 0.004431848411938341, 0.00013383022576490326);
uniform bool horizontal;
void main()
{
	vec4 ret = vec4(0,0,0,0);

	int y = 0;
	if(horizontal) {
		for(int i=-4;i<=4;i++) {
		ret += weights[4+i]*tex(pprocess_in, coords, ivec2(2*i, 0) );    
		}
	} else {
		for(int i=-4;i<=4;i++) {
		ret += weights[4+i]*tex(pprocess_in, coords, ivec2(0, 2*i) );    
		}
	}      
	color_out = ret;
	//color_out = vec4(1,0,0,1);
}

