out vec4 color_out;
in vec2 coords;
uniform sampler2D tex; // Color
uniform sampler2D tex2;// Normal
uniform sampler2D tex3;// Position
uniform sampler2D tex4;// Lighting
uniform sampler2D tex5;
uniform sampler2DShadow shadowTex; // Shadow
//uniform vec3 cameraPosition;
uniform int isLight;
//uniform vec3 lightPos;
vec3 fogColor = vec3(0.5, 0.5, 0.5);

const bool horizontal = true;




void main(){

	float diff3 = 0.001;
	/*
	vec3 image1 = vec3(texture(tex, coords + vec2(diff3, 0))).rgb;
	vec3 image2 = vec3(texture(tex, coords + vec2(0, diff3))).rgb;
	vec3 image3 = vec3(texture(tex, coords + vec2(0, -diff3))).rgb;
	vec3 image4 = vec3(texture(tex, coords + vec2(-diff3, 0))).rgb;
	vec3 image = (image1 + image2 + image3 + image4)/4;*/
	vec3 image = vec3(texture(tex, coords)).rgb;
	vec3 normal  = vec3(texture(tex2, coords)).rgb;
	vec3 position  = texture(tex3, coords).rgb;
	vec4 lighting  = texture(tex4, coords);




	//color_out.rgb = vec3(visibility);
	//color_out.a = 1;
	//color_out = vec4(1,1,0,1);
	//return;
    vec3 lightDir = lightPos - position.xyz ;

	normal = normalize(normal);
    lightDir = normalize(lightDir);


    

    vec3 eyeDir = normalize(cameraPosition-position.xyz);
    vec3 vHalfVector = normalize(reflect(cameraPosition - lightPos, normal));
    
	float dist = clamp(distance(cameraPosition, position)/300,0,1);
	float distCenter = clamp(distance(vec3(8,8,0), position)/128,0,1);
	float lightDist = clamp(distance(cameraPosition, lightPos)/300,0,1);
	fogColor = clamp(vec3(0.5,0.5,0.5)*(1 - lightDist), 0, 1);

	vec4 shadowCoord = lightMatrix * vec4(position,1.0);
	shadowCoord /= shadowCoord.w;
	shadowCoord = 0.5*shadowCoord + 0.5;

		float visibility = 0.7;
		//float bias = 0.005; 
	shadowCoord.z -= 0.01;
	//shadowCoord.z += max(0.1 * (dot(normal, lightDir)), 0.05); 
	//visibility = texture( shadowTex, shadowCoord.xyz );

	visibility = 0.0;

	visibility = texture(shadowTex, shadowCoord.xyz);        

	if(distance(position.xyz, cameraPosition.xyz) > 5)
	{
	//	image = vec3(blur(tex, coords));
	}

	color_out.rgb =  image * (0.3 + clamp(2*dot(normal, -lightDir), 0, 2)) * visibility;
	//return;
	vec2 diff = 1 / textureSize(tex3, 0);

	vec3 pos1 = vec3(texture(tex3, coords + vec2(diff.x, 0))).rgb;
	vec3 pos2 = vec3(texture(tex3, coords + vec2(0, diff.y))).rgb;
	vec3 pos3 = vec3(texture(tex3, coords + vec2(0, -diff.y))).rgb;
	vec3 pos4 = vec3(texture(tex3, coords + vec2(-diff.x, 0))).rgb;

	vec2 diff2 = 1 / textureSize(tex2, 0);;
	vec3 norm1 = vec3(texture(tex2, coords + vec2(diff2.x, 0))).rgb;
	vec3 norm2 = vec3(texture(tex2, coords + vec2(0, diff2.y))).rgb;
	vec3 norm3 = vec3(texture(tex2, coords + vec2(0, -diff2.y))).rgb;
	vec3 norm4 = vec3(texture(tex2, coords + vec2(-diff2.x, 0))).rgb;

	const float delta = 0.9;
	if(distance(position, pos1) > delta
	|| distance(position, pos2) > delta 
	|| distance(position, pos3) > delta 
	|| distance(position, pos4) > delta
	|| distance(normal, norm1) > delta
	|| distance(normal, norm2) > delta 
	|| distance(normal, norm3) > delta 
	|| distance(normal, norm4) > delta)
	{
		color_out.rgb = vec3(0,0,0);
	}


	//color_out.rgb = mix(fogColor, color_out.rgb, 1-dist);
	color_out.rgb = mix(color_out.rgb, 10*lighting.rgb, 0.1);
	//color_out.rgb *= 16;
	vec3 x = color_out.rgb;
	x = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);
	//color_out.rgb = 1.0/(color_out.rgb + vec3(1.0));
	const float gamma = 2.2;
	//color_out.rgb = pow(color_out.rgb, vec3(1.0 / gamma));
	color_out.rgb = x;
	//color_out.rgb = vec3(ivec3(15*color_out.rgb)/5);
	color_out.rgb = color_out.rgb *vec3(visibility);
	//color_out = mix(color_out, lighting, lighting.a);
	//color_out = lighting;
	//color_out.rgb = texture(tex5, coords).rga;
		color_out.a = 1;
	//color_out.rgb = shadowCoord.rgb;
	//color_out.rgb = vec3(1,1,1)*(distance(normal, norm1) + distance(normal, norm2) + distance(normal, norm3) + distance(normal, norm4));
}