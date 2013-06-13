#version 120

uniform sampler2D LOS_diffuse_texture_0;
uniform sampler2D LOS_diffuse_texture_1;
uniform sampler1D LOS_diffuse_texture_2;
uniform sampler1D LOS_diffuse_texture_3;
uniform vec4 LOS_scene_ambient;
uniform vec4 LOS_material_diffuse;
uniform vec4 LOS_material_celshading;
uniform float LOS_material_shininess;
uniform vec3 LOS_light_direction[LIGHTS];
uniform vec4 LOS_light_diffuse[LIGHTS];
uniform vec4 LOS_light_specular[LIGHTS];
uniform vec4 LOS_light_equation[LIGHTS];
uniform vec4 LOS_light_spotparams[LIGHTS];

varying vec3 F_normal;
varying vec2 F_texcoord;
varying vec3 F_eyev;
varying vec3 F_lightv[LIGHTS];

vec3 los_blinn_phong(in vec3 lv, in vec3 ev, in vec3 ld, in vec4 eq, in vec3 normal, in float shininess);
vec3 los_cel_shading(in vec4 material, in vec4 diff, in vec4 spec, in vec4 p, in sampler1D t1, in sampler1D t2);

void main()
{
	vec3 normal = normalize(F_normal);
	vec4 diffuse1 = texture2D(LOS_diffuse_texture_0, F_texcoord);
	vec4 diffuse = vec4(mix(LOS_material_diffuse.rgb, diffuse1.rgb, diffuse1.a), 1.0);
	vec4 diff = LOS_scene_ambient;
	vec4 spec = vec4(0.0);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		vec3 l = los_blinn_phong(F_lightv[i], F_eyev, LOS_light_direction[i],
			LOS_light_equation[i], normal, LOS_material_shininess);
		diff += l.z * l.x * LOS_light_diffuse[i];
		spec += l.z * l.y * LOS_light_specular[i];
	}
	vec3 color = los_cel_shading(diffuse, diff, spec,
		LOS_material_celshading, LOS_diffuse_texture_2, LOS_diffuse_texture_3);
#ifdef ENABLE_MRT
	gl_FragData[0] = vec4(color, 1.0);
	gl_FragData[1] = vec4(gl_FragCoord.z);
#else
	gl_FragColor = vec4(color, 1.0);
#endif
}
