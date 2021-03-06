--- Obstacle specification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.obstacle
-- @alias ObstacleSpec

local Class = require("system/class")
local EffectProfileSpec = require("core/specs/effect-profile")
local Spec = require("core/specs/spec")

--- Obstacle specification.
-- @type ObstacleSpec
local ObstacleSpec = Spec:register("ObstacleSpec", "obstacle", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "collision_group", type = "number", default = 0x1000, description = "Collision group."},
	{name = "collision_mask", type = "number", default = 0xFF, description = "Collision mask."},
	{name = "constraints", type = "list", list = {type = "string", details = {spec = "ConstraintSpec"}}, default = {}, description = "List of constraints."},
	{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "DialogSpec"}},
	{name = "effects", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of effect profiles.", details = {values = {spec = "EffectProfileSpec"}}},
	{name = "harvest_behavior", type = "string", default = "keep", description = "Harvest behavior: keep/destroy."},
	{name = "harvest_effect", type = "string", description = "Effect to play when harvested."},
	{name = "harvest_materials", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of harvestable materials.", details = {keys = {spec = "ItemSpec"}}},
	{name = "important", type = "boolean", default = true, description = "True to not allow the objectacle to be cleaned up from the map."},
	{name = "destroy_actions", type = "list", list = {type = "string"}, default = {}, description = "List of actions to perform when the obstacle is destroyed."},
	{name = "destroy_items", type = "list", list = {type = "struct", struct = {copy_velocity = {type = "boolean"}, name = {type = "string"}, position = {type = "vector"}}}, default = {}, description = "List of items to spawn when the obstacle is destroyed."},
	{name = "health", type = "number", description = "Number of hit points the obstacle has."},
	{name = "interactive", type = "boolean", default = true, description = "False to make the object not appear interactive."},
	{name = "marker", type = "string", description = "Map marker name."},
	{name = "mass", type = "number", default = 10, description = "Mass in kilograms."},
	{name = "model", type = "string", description = "Model to use for the obstacle."},
	{name = "physics", type = "string", default = "static", description = "Physics mode."},
	{name = "special_effects", type = "list", list = {type = "string", details = {value = {spec = "EffectSpec"}}}, default = {}, description = "List of special effects to render."},
	{name = "usages", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of ways how the object can be used.", details = {keys = {spec = "ActionSpec"}}},
	{name = "vulnerabilities", type = "dict", dict = {type = "number"}, description = "Dictionary of damage vulnerabilities."}
})

--- Creates a new obstacle specification.
-- @param clss ObstacleSpec class.
-- @param args Arguments.
-- @return New item specification.
ObstacleSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	self.harvest_enabled = args.harvest_materials ~= nil
	return self
end

--- Finds the constraints of the obstacle.
-- @param self Obstacle spec.
-- @return List of constraint specs, or nil.
ObstacleSpec.get_constraints = function(self)
	if not self.constraints then return end
	local res = {}
	for k,v in pairs(self.constraints) do
		local con = Main.specs:find_by_name("ConstraintSpec", v)
		if con then
			table.insert(res, con)
		end
	end
	if #res == 0 then return end
	return res
end

--- Gets an effect by name.
-- @param self ItemSpec.
-- @param name Effect name.
-- @param profile Effect profile name. Nil for "default".
-- @return Effect spec, or nil.
ObstacleSpec.get_effect = function(self, name, profile)
	local try = function(self, p, e)
		local pname = self.effects[p]
		if not pname then return end
		local profile = Main.specs:find_by_name("EffectProfileSpec", pname)
		if not profile then return end
		return profile:get_effect(e)
	end
	return profile and try(self, profile, name) or try(self, "default", name)
end

--- Finds the special effects of the obstacle.
-- @param self Obstacle spec.
-- @return List of effect specs, or nil.
ObstacleSpec.get_special_effects = function(self)
	if not self.special_effects then return end
	local res = {}
	for k,v in pairs(self.special_effects) do
		local eff = Main.specs:find_by_name("EffectSpec", v)
		if eff then
			table.insert(res, eff)
		end
	end
	if #res == 0 then return end
	return res
end

--- Gets the use actions applicable to the obstacle.
-- @param self Obstacle spec.
-- @return List of actions specs.
ObstacleSpec.get_use_actions = function(self)
	local res = {}
	for k,v in pairs(self.usages) do
		local a = Main.specs:find_by_name("ActionSpec", k)
		if a then table.insert(res, a) end
	end
	return res
end

--- Returns a random obstacle spec.
-- @param clss ObstacleSpec class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li></ul>
-- @return ObstacleSpec or nil.
ObstacleSpec.random = function(clss, args)
	if args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		return cat[math.random(#cat)]
	end
	if args and args.name then
		return clss.dict_name[args.name]
	end
	return clss.dict_id[math.random(#clss.dict_id)]
end

return ObstacleSpec
