require(Mod.path .. "spec")
require "common/string"

CraftingRecipeSpec = Class(Spec)
CraftingRecipeSpec.type = "crafting recipe"
CraftingRecipeSpec.dict_id = {}
CraftingRecipeSpec.dict_cat = {}
CraftingRecipeSpec.dict_name = {}
CraftingRecipeSpec.introspect = Introspect{
	name = "Itemspec",
	fields = {
		{name = "name", type = "string", description = "Name of the crafted item.", details = {spec = "Itemspec"}},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "count", type = "number", default = 1, description = "Number of items crafted per use.", details = {integer = true, min = 1}},
		{name = "effect", type = "string", description = "Name of the effect to play when the item is crafted.", details = {spec = "Effectspec"}},
		{name = "materials", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of crafting materials.", details = {keys = {spec = "Itemspec"}, values = {integer = true, min = 1}}},
		{name = "modes", type = "dict", dict = {type = "boolean"}, default = {["default"] = true}, description = "Crafting modes that can be used for crafting the item."}}}

--- Creates a new crafting recipe specification.
-- @param clss CraftingRecipeSpec class.
-- @param args Arguments.
-- @return New crafting recipe specification.
CraftingRecipeSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end