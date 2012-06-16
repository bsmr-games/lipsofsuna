#version 120

uniform mat4 LOS_matrix_modelviewproj;
uniform mat4 LOS_matrix_world_inverse;
uniform mat4x3 LOS_skeletal_matrix[BONES];

attribute vec3 vertex;
attribute vec4 blendIndices;
attribute vec4 blendWeights;

void LOS_skeletal_animation_nonmltan(
	in vec3 vertex, in mat4 inverse,
	out vec3 vertex_res)
{
	vec3 v = vec3(0.0);
	float total = 0.0;
	int bones[4] = int[](int(blendIndices.x), int(blendIndices.y), int(blendIndices.z), int(blendIndices.w));
	float weights[4] = float[](blendWeights.x, blendWeights.y, blendWeights.z, blendWeights.w);
	for(int i = 0 ; i < 4 ; i++)
	{
		v += weights[i] * LOS_skeletal_matrix[bones[i]] * vec4(vertex, 1.0);
		total += weights[i];
	}
	vertex_res = (inverse * vec4(v / total, 1.0)).xyz;
}

void main()
{
	vec3 t_vertex;
	LOS_skeletal_animation_nonmltan(vertex, LOS_matrix_world_inverse, t_vertex);
	gl_Position = LOS_matrix_modelviewproj * vec4(t_vertex,1.0);
}
