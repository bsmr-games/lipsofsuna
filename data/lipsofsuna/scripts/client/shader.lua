local instfunc = Shader.new
Shader.dict_name = {}

--- Creates a new shader and enables it.<br/>
-- The shader isn't compiled at all when it's created. It's done later in the
-- options screen when the used shader quality is known. Until then we only
-- store the settings for the different quality levels.
-- @param clss Shader class.
-- @param args Arguments.<ul>
--   <li>high: High quality shader.</li>
--   <li>low: Low quality shader.</li>
--   <li>medium: Medium quality shader.</li>
--   <li>name: Shader name.</li></ul>
-- @return New shader.
Shader.new = function(clss, args)
	local self = instfunc(clss, args)
	if self then
		for k,v in pairs(args) do self[k] = v end
		self:enable()
		return self
	end
end

--- Disables the shader.<br/>
-- The shader will be subject to normal garbage collection when disabled.
-- @param self Shader.
Shader.disable = function(self)
	Shader.dict_name[self.name] = nil
end

--- Enables the shader.<br/>
-- The shader will not be subject to garbage collection when enabled.
-- @param self Shader.
Shader.enable = function(self)
	Shader.dict_name[self.name] = self
end

--- Sets the quality level of the shader.<br/>
-- Older graphics cards can't deal with certain effects, such as geometry
-- amplification in the geometry shader, so multiple shader sets are used.
-- This function allows switching the shader quality be recompiling the
-- shader with different settings.
-- \param self Shader.
-- \param value Integer in the range of [1,3].
Shader.set_quality = function(self, value)
	local qualities = {self.low, self.medium, self.high}
	if self.quality ~= value then
		self.quality = value
		-- Try the requested level.
		local q = qualities[value]
		if q then return self:compile(q) end
		-- Fallback to lower setting.
		for i = value,1,-1 do
			local q = qualities[i]
			if q then return self:compile(q) end
		end
		-- Fallback to higher setting.
		for i = value,3 do
			local q = qualities[i]
			if q then return self:compile(q) end
		end
	end
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
-- @param sp Variable name that contains additional specular color.
Shader.los_lighting_default = function(co, no, lv, hv, sp)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, dot(%s, normalize(lv)));
		float coeff = max(0.0, dot(%s, normalize(%s)));
		float fspec = pow(max(0.0, coeff), LOS_material_shininess);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fdiff * fspec * LOS_light[lighting_index].specular.rgb * %sLOS_material_specular.rgb);
	}]], lv, no, no, hv, sp and (sp .. ".rgb * ") or "")
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param ta Variable name that contains the fragment tangent.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
Shader.los_lighting_hair = function(co, no, ta, lv, hv)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, 0.25 + 0.75 * dot(%s, normalize(lv)));
		float coeff1 = max(0.0, dot(%s, normalize(%s)));
		float tanref = max(0.0, dot(%s, reflect(-normalize(%s), %s)));
		float coeff2 = sqrt(1.0 - tanref * tanref);
		float coeff = mix(coeff1, coeff2, 0.6);
		float fspec = pow(coeff, LOS_material_shininess);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fdiff * fspec * LOS_light[lighting_index].specular.rgb * LOS_material_specular.rgb);
	}]], lv, no, no, hv, ta, co, no)
end

--- Calculates the light vectors and half vectors.
-- @param lv Array name where to return light vectors.
-- @param hv Array name where to return half vectors.
-- @oaran co Variable name that contains the transformed vertex coordinate.
Shader.los_lighting_vectors = function(lv, hv, co)
	return string.format([[int lighting_vindex;
	for(lighting_vindex = 0 ; lighting_vindex < LOS_LIGHT_MAX ; lighting_vindex++)
	{
		vec3 lighting_vector = LOS_light[lighting_vindex].position_premult - %s;
		%s[lighting_vindex] = lighting_vector;
		%s[lighting_vindex] = normalize(lighting_vector - %s);
	}]], co, lv, hv, co)
end

Shader.los_normal_mapping = [[
vec3 los_normal_mapping(in vec3 normal, in vec3 tangent, in vec2 texcoord, in sampler2D sampler)
{
	vec3 tmp = normalize(normal);
	mat3 tangentspace = mat3(tangent, cross(tmp, tangent), tmp);
	vec3 n = normalize(texture(sampler, texcoord).xyz * 2.0 - 1.0);
	return normalize(tangentspace * n);
}]]

Shader.los_triangle_tangent = [[
vec3 los_triangle_tangent(in vec3 co0, in vec3 co1, in vec3 co2, in vec2 uv0, in vec2 uv1, in vec2 uv2)
{
	vec3 ed0 = co1 - co0;
	vec3 ed1 = co2 - co0;
	return normalize(ed1 * (uv1.y - uv0.y) - ed0 * (uv2.y - uv0.y));
}]]

Shader.los_shadow_mapping = [[
float los_shadow_mapping(in vec4 lightcoord, in sampler2DShadow sampler)
{
	float esm_c = 80.0;
	float esm_d = lightcoord.z;
	float esm_z = lightcoord.w * textureProj(sampler, lightcoord);
	float esm_f = exp(-esm_c * esm_d + esm_c * esm_z);
	return clamp(esm_f, 0.0, 1.0);
}]]

Shader.los_shadow_mapping_pcf = [[
float los_shadow_mapping_pcf(in vec4 lightcoord, in sampler2DShadow sampler)
{
	vec2 pcf = vec2(clamp(0.004 * lightcoord.z, 0.02, 0.2), 0.0);
	float blend = los_shadow_mapping(lightcoord, sampler);
	blend += los_shadow_mapping(lightcoord - pcf.xyyy, sampler);
	blend += los_shadow_mapping(lightcoord + pcf.xyyy, sampler);
	blend += los_shadow_mapping(lightcoord - pcf.yxyy, sampler);
	blend += los_shadow_mapping(lightcoord + pcf.yxyy, sampler);
	blend *= 0.2;
	return blend;
}]]
