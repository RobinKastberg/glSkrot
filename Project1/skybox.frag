out vec4 out_Color;

in vec2 v_TexCoord;
in vec3 eyeDirection;
uniform samplerCube tex;


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

	return min(min(sphere(position, vec3(0,0,0), 0.1),
				cube(position, vec3(0.3,0,0), 0.1)),position.y+1);
}
vec3 grad(in vec3 position)
{
	float h = 0.0001;
	vec3 ret;
	ret.x = (dist(position+vec3(h,0,0)) - dist(position))/h;
	ret.y = (dist(position+vec3(0,h,0)) - dist(position))/h;
	ret.z = (dist(position+vec3(0,0,h)) - dist(position))/h;

	return normalize(ret);
}
void main()
{
	vec4 ambient = vec4(158.0/255.0, 179.0/255.0, 221.0/255.0, 1.0);
	vec4 gradient = mix(vec4(6.0/255.0, 36.0/255.0, 80.0/255.0, 1.0),
					    vec4(132.0/255.0, 139.0/255.0, 255.0/255.0, 1.0),
						0.5*(-eyeDirection.z+1)
						);

	vec3 ray = vec3(2*v_TexCoord.xy-1,-1);
	//ray.y -= 0.4;
	//vec3 pos = vec3(0,1,1);
	vec3 pos = cameraPosition.xyz;
	//vec3 ray = eyeDirection.xyz;
    // For each iteration, we read from our noise function the density of our current position, and adds it to this density variable.
	float density = 0;
    int _Iterations = 20;
	float _CloudDensity = 0.5;    
	float _ViewDistance = 0.01;
	float t = 0.0;        
    for(int i = 0; i < _Iterations; ++i)
    {
        vec3 p = pos + ray * t;
        float d = 0.7-texture(tex, p).r; //dist(p); // Distance to sphere of radius 0.5
        if(d < 0.0001)
        {
            out_Color = i*vec4(1.0)/20; // Sphere color
			//out_Color.rgb = vec3(1,1,0); //grad(p); //normalize(p);
			//out_Color.a = 1;
            return;
        }

        t += d;
    }
	discard;
	out_Color = vec4(0,0,0,1);
    // And here i just melted all our variables together with random numbers until I had something that looked good.
    // You can try playing around with them too.
	//vec4 _CloudColor = vec4(0.8, 0.8, 0.8, 0.5);
	//out_Color.rgb = gradient.rgb + (_CloudColor.rgb - 0.5) * (density / _Iterations) * 20 * _CloudColor.a;

	out_Color.a = 1;
}