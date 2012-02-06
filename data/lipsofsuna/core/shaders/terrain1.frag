#version 120

uniform sampler2D LOS_diffuse_texture_0;
uniform sampler2D LOS_diffuse_texture_1;
uniform sampler2D LOS_diffuse_texture_2;
uniform sampler2D LOS_shadow_texture_0;
uniform sampler2D LOS_shadow_texture_1;
uniform sampler2D LOS_shadow_texture_2;
uniform vec4 LOS_scene_ambient;
uniform vec4 LOS_material_diffuse;
uniform float LOS_material_shininess;
uniform vec3 LOS_light_direction[LIGHTS];
uniform vec4 LOS_light_diffuse[LIGHTS];
uniform vec4 LOS_light_specular[LIGHTS];
uniform vec4 LOS_light_equation[LIGHTS];
uniform vec4 LOS_light_spotparams[LIGHTS];

varying vec3 F_normal;
varying vec3 F_tangent;
varying vec2 F_texcoord;
varying vec4 F_color;
varying float F_splatting;
varying vec3 F_lightv[LIGHTS];
varying vec3 F_lighthv[LIGHTS];
varying vec4 F_shadow[LIGHTS];

vec3 los_normal_mapping(in vec3 normal, in vec3 tangent, in vec4 sample)
{
	vec3 nml1 = normalize(normal);
	if(length(tangent) < 0.01) return nml1;
	vec3 tan1 = normalize(tangent);
	if(abs(dot(nml1, tan1)) > 0.9) return nml1;
	mat3 tangentspace = mat3(tan1, cross(tan1, nml1), nml1);
	vec3 n = tangentspace * (sample.xyz * 2.0 - vec3(1.0));
	if(length(n) < 0.01) return nml1;
	return normalize(n);
}
vec3 los_blinn_phong(in vec3 lv, in vec3 hv, in vec3 ld, in vec4 eq,
	in vec3 normal, in vec4 spotparam, in float shininess)
{
	vec3 lvn = normalize(lv);
	float diff = dot(normal, lvn);
	if(diff <= 0.0)
		return vec3(0.0);
	float dist = length(lv);
	float att = 1.0 / dot(eq.yzw, vec3(1.0, dist, dist * dist));
	float ndh = dot(normal, normalize(hv));
	float spec = pow(max(0.0, ndh), shininess);
	float spot = max(0.0, -dot(ld, lvn));
	att *= pow(smoothstep(spotparam.y, spotparam.x, spot), spotparam.z);
	return vec3(diff, spec, att);
}

void main()
{
	vec4 normalmap = texture2D(LOS_diffuse_texture_1, F_texcoord);
	vec3 normal = los_normal_mapping(F_normal, F_tangent, normalmap);
	vec4 diffuse0 = texture2D(LOS_diffuse_texture_0, F_texcoord);
	vec4 diffuse1 = texture2D(LOS_diffuse_texture_2, F_texcoord);
	vec4 diffuse = vec4(F_color.rgb, 1.0) * mix(diffuse0, diffuse1, F_splatting);
	float shadows[3];
	shadows[0] = texture2DProj(LOS_shadow_texture_0, F_shadow[0]).x;
	shadows[1] = texture2DProj(LOS_shadow_texture_1, F_shadow[1]).x;
	shadows[2] = texture2DProj(LOS_shadow_texture_2, F_shadow[2]).x;
	vec4 light = LOS_scene_ambient;
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		vec3 l = los_blinn_phong(F_lightv[i], F_lighthv[i],
			LOS_light_direction[i], LOS_light_equation[i], normal,
			LOS_light_spotparams[i], LOS_material_shininess);
		light += l.z * shadows[i] * (l.x * LOS_light_diffuse[i] + l.y * LOS_light_specular[i]);
	}
	gl_FragColor = LOS_material_diffuse * diffuse * light;
}