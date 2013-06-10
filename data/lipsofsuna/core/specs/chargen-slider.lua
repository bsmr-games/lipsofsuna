--- Character creator slider spec.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.chargen_slider
-- @alias ChargenSliderSpec

local Class = require("system/class")
local Spec = require(Mod.path .. "spec")

--- Character creator slider spec.
-- @type ChargenSliderSpec
ChargenSliderSpec = Class("ChargenSliderSpec", Spec)
ChargenSliderSpec.type = "chargen slider"
ChargenSliderSpec.dict_id = {}
ChargenSliderSpec.dict_cat = {}
ChargenSliderSpec.dict_name = {}
ChargenSliderSpec.introspect = Introspect{
	name = "ChargenSliderSpec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories. Used in the user interface for grouping."},
		{name = "field_name", type = "string", description = "Name of the actor field connected to the slider."},
		{name = "field_index", type = "number", description = "Index of the actor field connected to the slider."},
		{name = "invert", type = "boolean", default = false, description = "Inversion of the slider."},
		{name = "shape_keys", type = "list", list = {type = "string"}, description = "List of shape key names."},
		{name = "type", type = "string", description = "Slider type."}
	}}

--- Registers a spec.
-- @param clss ChargenSliderSpec class.
-- @param args Arguments.
-- @return Spec.
ChargenSliderSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

return ChargenSliderSpec
