local Model = require("system/model")

if not Los.program_load_extension("model-editing") then
	error("loading extension `model-editing' failed")
end

------------------------------------------------------------------------------

--- Adds a material to the model.
-- @param self Model.
-- @param ... Arguments.<ul>
--   <li>cull: False to disable face culling.</li>
--   <li>shader: Shader name.</li></ul>
Model.add_material = function(self, ...)
	Los.model_add_material(self.handle, ...)
end

--- Adds triangles to the model.
-- @param self Model.
-- @param ... Arguments.<ul>
--   <li>material: Material index.</li>
--   <li>vertices: Array of vertices.</li></ul>
Model.add_triangles = function(self, ...)
	Los.model_add_triangles(self.handle, ...)
end

--- Calculates LOD data for the model.
-- @param self Model.
-- @param ... Arguments.<ul>
--   <li>1: Number of levels.</li>
--   <li>2: Minimum detail level.</li></ul>
-- @return True on success.
Model.calculate_lod = function(self, ...)
	Los.model_calculate_lod(self.handle, ...)
end

--- Edits the matching material.
-- @param self Model.
-- @param args Arguments.<ul>
--   <li>diffuse: Diffuse color to set, or nil.</li>
--   <li>match_shader: Shader name to match, or nil.</li>
--   <li>match_texture: Texture name to match, or nil.</li>
--   <li>material: Material reference name to set, or nil.</li>
--   <li>specular: Specular color to set, or nil.</li>
--   <li>shader: Shader to set, or nil.</li>
--   <li>textures: Array of texture to set, or nil.</li></ul>
Model.edit_material = function(self, args)
	Los.model_edit_material(self.handle, args)
end

--- Merges a morphed copy of the given model to this model.
-- @param self Model.
-- @param model Model.
-- @param targets List of alternating morph target names and influence.
Model.merge_morph = function(self, model, targets)
	Los.model_merge_morph(self.handle, model.handle, unpack(targets))
end

--- Morphs a model with one of its shape keys.
-- @param self Model.
-- @param ... Arguments.<ul>
--   <li>1,shape: Shape key name.</li>
--   <li>2,value: Shape influence multiplier.</li>
--   <li>3,ref: Reference model for relative morphing, or nil.</li></ul>
Model.morph = function(self, ...)
	local a,b,c = ...
	if type(a) == "table" then
		Los.model_morph(self.handle, a.shape, a.value, a.ref and a.ref.handle)
	else
		Los.model_morph(self.handle, a, b, c and c.handle)
	end
end

--- Creates a morphed copy of the model.
-- @param self Model.
-- @param targets List of alternating morph target names and influence.
-- @return Copied and morphed model.
Model.morph_copy = function(self, targets)
	local copy = Class.new(Model)
	copy.handle = Los.model_morph_copy(self.handle, unpack(targets))
	if self.name then
		copy.name = "*" .. self.name
	end
	return copy
end

--- Removes the vertices and indices of the model.
-- @param self Model.
Model.remove_vertices = function(self)
	Los.model_remove_vertices(self.handle)
end

return Model
