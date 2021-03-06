--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.crafting_recipe
-- @alias CraftingRecipeSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type CraftingRecipeSpec
local CraftingRecipeSpec = Spec:register("CraftingRecipeSpec", "crafting recipe", {
	{name = "name", type = "string", description = "Name of the crafted item.", details = {spec = "ItemSpec"}},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "count", type = "number", default = 1, description = "Number of items crafted per use.", details = {integer = true, min = 1}},
	{name = "effect", type = "string", description = "Name of the effect to play when the item is crafted.", details = {spec = "EffectSpec"}},
	{name = "materials", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of crafting materials.", details = {keys = {spec = "ItemSpec"}, values = {integer = true, min = 1}}},
	{name = "modes", type = "dict", dict = {type = "boolean"}, default = {["default"] = true}, description = "Crafting modes that can be used for crafting the item."}
})

--- Creates a new crafting recipe specification.
-- @param clss CraftingRecipeSpec class.
-- @param args Arguments.
-- @return New crafting recipe specification.
CraftingRecipeSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

return CraftingRecipeSpec
