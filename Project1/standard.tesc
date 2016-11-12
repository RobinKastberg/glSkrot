layout(vertices = 4) out;
in vec3 v_eyeNormal[];
out vec3 tc_eyeNormal[];
in vec3 v_modelNormal[];
out vec3 tc_modelNormal[];
in vec4 v_Position[];
out vec4 tc_Position[];
void main()
{
 float max_tesselation = 1024;
 float i1 = length(cameraPosition - gl_in[0].gl_Position);
 float i2 = length(cameraPosition - gl_in[1].gl_Position);
 float i3 = length(cameraPosition - gl_in[2].gl_Position);
 float i4 = length(cameraPosition - gl_in[3].gl_Position);
 float dist = min(i1,min(i2,min(i3,i4)));
 max_tesselation /= clamp(exp(dist),1,1024);
 gl_TessLevelOuter[0] = max_tesselation;
 gl_TessLevelOuter[1] = max_tesselation;
 gl_TessLevelOuter[2] = max_tesselation;
 gl_TessLevelOuter[3] = max_tesselation;
 
 gl_TessLevelInner[0] = max_tesselation;
 gl_TessLevelInner[1] = max_tesselation;
 
 gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
 tc_eyeNormal[gl_InvocationID] = v_eyeNormal[gl_InvocationID];
 tc_modelNormal[gl_InvocationID] = v_modelNormal[gl_InvocationID];
 tc_Position[gl_InvocationID] = v_Position[gl_InvocationID];
}