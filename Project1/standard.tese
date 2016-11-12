
layout(quads, equal_spacing, ccw) in;

in vec3 tc_eyeNormal[];
out vec3 v_eyeNormal;

in vec3 tc_modelNormal[];
out vec3 v_modelNormal;

in vec4 tc_Position[];
out vec4 v_Position;

out vec2 v_texCoord;
uniform sampler2D texture1;

//quad interpol
vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3)
{
 vec4 a = mix(v0, v1, gl_TessCoord.x);
 vec4 b = mix(v3, v2, gl_TessCoord.x);
 return mix(a, b, gl_TessCoord.y);
}

vec3 interpolate(in vec3 v0, in vec3 v1, in vec3 v2, in vec3 v3)
{
 vec3 a = mix(v0, v1, gl_TessCoord.x);
 vec3 b = mix(v3, v2, gl_TessCoord.x);
 return mix(a, b, gl_TessCoord.y);
}
 
void main()
{ 
 gl_Position = interpolate(
  gl_in[0].gl_Position, 
  gl_in[1].gl_Position, 
  gl_in[2].gl_Position, 
  gl_in[3].gl_Position);

   v_Position = interpolate(
  tc_Position[0], 
  tc_Position[1], 
  tc_Position[2], 
  tc_Position[3]);

  v_texCoord = fract(v_Position.xy);

  gl_Position.z += 0.5*texture(texture1, v_texCoord).r;
  //gl_Position.z += 0.1*texture(texture1, 20*v_texCoord).r;
  v_Position.z = gl_Position.z;
  v_Position = modelMatrix[currentModel] * v_Position;
  gl_Position = projectionMatrix * viewMatrix * modelMatrix[currentModel] * gl_Position;
   v_eyeNormal = interpolate(
  tc_eyeNormal[0], 
  tc_eyeNormal[1], 
  tc_eyeNormal[2], 
  tc_eyeNormal[3]);
     v_modelNormal = interpolate(
  tc_modelNormal[0], 
  tc_modelNormal[1], 
  tc_modelNormal[2], 
  tc_modelNormal[3]);
}