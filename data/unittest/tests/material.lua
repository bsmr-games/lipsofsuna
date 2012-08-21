Unittest:add(1, "system", "material", function()
	local Material = require("system/material")
	local m = Material()
	assert(type(m:get_cullface()) == "boolean")
	assert(type(m:get_diffuse()) == "table")
	assert(type(m:get_flags()) == "number")
	assert(type(m:get_friction()) == "number")
	assert(type(m:get_id()) == "number")
	assert(type(m:get_name()) == "string")
	assert(type(m:get_shader()) == "string")
	assert(type(m:get_shininess()) == "number")
	assert(type(m:get_specular()) == "table")
	assert(type(m:get_texture()) == "table")
	assert(type(m:get_texture_scale()) == "number")
	assert(type(m:get_type()) == "string")
end)
