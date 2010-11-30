Shader{
name = "hair",
sort = true,
pass1_color_write = false,
pass1_depth_func = "lequal",
pass1_vertex = [[
out fragvar
{
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
in fragvar
{
	vec2 texcoord;
} IN;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	if (diffuse.a < 0.90)
		discard;
}]],
pass6_blend = true,
pass6_blend_src = "src_alpha",
pass6_blend_dst = "one",
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_vertex = [[
out fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	OUT.coord = tmp.xyz;
	OUT.lightvector = LOS_light_position_premult - tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.tangent = LOS_matrix_normal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_fragment = [[
in fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_specular
.. Shader.los_normal_mapping .. [[
float los_light_diffuse_hair(in vec3 coord, in vec3 normal)
{
	vec3 dir = normalize(coord);
	float coeff = dot(normal, dir);
	return max(0.0, 0.25 + 0.75 * coeff);
}
float los_light_specular_hair(in vec3 coord, in vec3 normal, in vec3 tangent, in float shininess)
{
	vec3 refl = reflect(-normalize(coord), normal);
	float a = max(0.0, dot(tangent, refl));
	float coeff1 = max(0.0, dot(normal, refl));
	float coeff2 = sqrt(1.0 - a * a);
	float coeff = mix(coeff1, coeff2, 0.6);
	return pow(coeff, shininess);
}
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, LOS_diffuse_texture_1);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	if(normal.z < 0)
		normal = -normal;
	float fattn = los_light_attenuation(IN.lightvector, LOS_light_equation);
	float fdiff = los_light_diffuse_hair(IN.lightvector, normal);
	float fspec = los_light_specular_hair(IN.coord, normal, tangent, LOS_material_shininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, LOS_light_ambient,
		LOS_light_diffuse, LOS_light_specular * LOS_material_specular);
	gl_FragColor = LOS_material_diffuse * diffuse * light;
}]]}
