--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.spell
-- @alias Spellspec

local Class = require("system/class")
require(Mod.path .. "spec")

--- TODO:doc
-- @type Spellspec
Spellspec = Class("Spellspec", Spec)
Spellspec.type = "spell"
Spellspec.dict_id = {}
Spellspec.dict_cat = {}
Spellspec.dict_name = {}
Spellspec.introspect = Introspect{
	name = "Spellspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "model", type = "string", description = "Model to use for the spell."},
		{name = "particle", type = "string", description = "Particle effect to use for the spell."}
	}}

--- Creates a new spell specification.
-- @param clss Spellspec class.
-- @param args Arguments.<ul>
--   <li>model: </li>
--   <li>name: </li>
--   <li>particle: </li></ul>
-- @return New item specification.
Spellspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end


