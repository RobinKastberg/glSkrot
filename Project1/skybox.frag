out vec4 out_Color;

in vec2 v_TexCoord;
in vec3 eyeDirection;
uniform samplerCube tex;
uniform sampler2D tex2d;


/*void main(){
	vec4 ambient = vec4(158.0/255.0, 179.0/255.0, 221.0/255.0, 1.0);
	vec4 gradient = mix(vec4(6.0/255.0, 36.0/255.0, 80.0/255.0, 1.0),
					    vec4(132.0/255.0, 139.0/255.0, 255.0/255.0, 1.0),
						0.5*(-eyeDirection.z+1)
						);
	float newZ = 3*eyeDirection.z;	
	out_Color = (ambient + gradient)/2.0;
	vec3 eye = eyeDirection;
	eye.x = cos(time.x)*eyeDirection.x + sin(time.x)*eyeDirection.z;
	eye.z = -sin(time.x)*eyeDirection.x+ cos(time.x)*eyeDirection.z;
	if(texture(tex, eye).r > 0.5)
		out_Color = mix((ambient + gradient)/2.0, vec4(1,1,1,1), texture(tex, eye).r-0.5);
	out_Color.a = 1;
}*/
float max3 (in vec3 v) {
  return max (max (v.x, v.y), v.z);
}
float sphere(in vec3 position, in vec3 center, in float radius)
{
	return length(position - center) - radius;
}
float cube(in vec3 position, in vec3 center, in float radius)
{
	return max3(abs(position - center)) - radius;
}
float dist(in vec3 position) {
	
	return 0.2*texture(tex, position.xyz).r+sphere(position, vec3(0,0,0), 1); // -0.1*texture(tex2d, 0.1*position.xy).r;//sphere(mod(position,1)-0.5, vec3(0,0,0), 0.3); //sphere(position, vec3(0,0,0), 0.3);
}
vec3 grad(in vec3 position)
{
	const float h = 0.01;
	vec3 ret;
	ret.x = (dist(position+vec3(h,0,0)) - dist(position+vec3(-h,0,0)))/(2*h);
	ret.y = (dist(position+vec3(0,h,0)) - dist(position+vec3(0,-h,0)))/(2*h);
	ret.z = (dist(position+vec3(0,0,h)) - dist(position+vec3(h,0,-h)))/(2*h);

	return normalize(ret);
}
void main()
{
	vec4 ambient = vec4(158.0/255.0, 179.0/255.0, 221.0/255.0, 1.0);
	vec4 gradient = mix(vec4(6.0/255.0, 36.0/255.0, 80.0/255.0, 1.0),
					    vec4(132.0/255.0, 139.0/255.0, 255.0/255.0, 1.0),
						0.5*(-eyeDirection.z+1)
						);

	float u = 2*v_TexCoord.x - 1;
	float v = 2*v_TexCoord.y - 1;
	vec3 pos = cameraPosition.xyz;
	mat4 vMatrix = transpose(viewMatrix);
	vec3 ray = normalize(u * vMatrix[0].xyz + v * vMatrix[1].xyz - vMatrix[2].xyz);
	//vec3 ray = vec3(2*v_TexCoord.xy-1,-1);
	//ray.y -= 0.4;
	//vec3 pos = vec3(0,1,1);
	//vec3 pos = cameraPosition.xyz;
    // For each iteration, we read from our noise function the density of our current position, and adds it to this density variable.
	float density = 0;
    int _Iterations = 100;
	float _CloudDensity = 0.5;    
	float _ViewDistance = 0.01;
	float t = 0.0;       
	vec3 lightPosition = vec3(1,1,1); 
    for(int i = 0; i < _Iterations; ++i)
    {
        vec3 p = pos + ray * t;
        float d = dist(p); // Distance to sphere of radius 0.5
        if(d < 0.01)
        {
            //out_Color = i*vec4(1.0)/20; // Sphere color
			out_Color = ambient + gradient * dot(normalize(lightPosition - p),grad(p)); //normalize(p);
			//out_Color.a = 1;
        }

        t += d;
    }
	return;
	discard;
	out_Color = vec4(0,0,0,1);
    // And here i just melted all our variables together with random numbers until I had something that looked good.
    // You can try playing around with them too.
	//vec4 _CloudColor = vec4(0.8, 0.8, 0.8, 0.5);
	//out_Color.rgb = gradient.rgb + (_CloudColor.rgb - 0.5) * (density / _Iterations) * 20 * _CloudColor.a;

	out_Color.a = 1;
}