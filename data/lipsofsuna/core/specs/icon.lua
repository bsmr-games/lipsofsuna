--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.icon
-- @alias IconSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type IconSpec
local IconSpec = Spec:register("IconSpec", "icon", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "image", type = "string", default = "icon1", description = "Source image name."},
	{name = "offset", type = "list", list = {type = "number"}, default = {0,0}, description = "Pixel offset in the source image."},
	{name = "size", type = "list", list = {type = "number"}, default = {32,32}, description = "Size in pixels."}
})

--- Creates a new icon specification.
-- @param clss Iconspec class.
-- @param args Arguments.
-- @return New icon specification.
IconSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

return IconSpec
