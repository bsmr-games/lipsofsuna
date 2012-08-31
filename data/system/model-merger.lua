--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.model_merger
-- @alias ModelMerger

local Class = require("system/class")
local Model = require("system/model")

if not Los.program_load_extension("model-merge") then
	error("loading extension `model-merge' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type ModelMerger
local ModelMerger = Class("ModelMerger")

--- Creates a new model merger.
-- @param clss ModelMerger class.
-- @return New model merger.
ModelMerger.new = function(clss)
	local self = Class.new(ModelMerger)
	self.handle = Los.merger_new()
	return self
end

--- Adds a model.
-- @param self Model merger.
-- @param model Model to add.
ModelMerger.add_model = function(self, model)
	Los.merger_add_model(self.handle, model.handle)
end

--- Adds a morphed model.
-- @param self Model merger.
-- @param model Model to add.
-- @param targets List of alternating morph target names and influence.
ModelMerger.add_model_morph = function(self, model, targets)
	Los.merger_add_model_morph(self.handle, model.handle, unpack(targets))
end

--- Queues finishing the build.
-- @param self Model merger.
ModelMerger.finish = function(self)
	Los.merger_finish(self.handle)
end

--- Pops a finished model.
-- @param self Model merger.
-- @return Model or nil.
ModelMerger.pop_model = function(self)
	local handle = Los.merger_pop_model(self.handle)
	if not handle then return end
	return Model:new_from_handle(handle)
end

--- Replaces fields of matching materials.
-- @param self Model merger.
-- @param args Arguments.<ul>
--   <li>diffuse: Diffuse color to set, or nil.</li>
--   <li>match_material: Material reference name to match.</li>
--   <li>material: Material reference name to set, or nil.</li>
--   <li>specular: Specular color to set, or nil.</li>
--   <li>textures: Array of texture to set, or nil.</li></ul>
ModelMerger.replace_material = function(self, args)
	Los.merger_replace_material(self.handle, args)
end

return ModelMerger

