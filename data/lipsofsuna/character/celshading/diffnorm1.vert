#version 120

uniform mat4 LOS_matrix_modelviewproj;
uniform vec3 LOS_camera_position;
uniform vec4 LOS_light_position[LIGHTS];

attribute vec3 vertex;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 uv0;

varying vec3 F_normal;
varying vec3 F_tangent;
varying vec2 F_texcoord;
varying vec3 F_eyev;
varying vec3 F_lightv[LIGHTS];

void main()
{
	F_normal = normal;
	F_tangent = tangent;
	F_texcoord = uv0;
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex,1.0);
	F_eyev = normalize(LOS_camera_position - vertex.xyz);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		F_lightv[i] = LOS_light_position[i].xyz - (vertex.xyz * LOS_light_position[i].w);
	}
}
