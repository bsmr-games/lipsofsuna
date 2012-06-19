Unittest:add(1, "material", function()
	require "system/tiles"
	local m = Material()
	assert(type(m.cullface) == "boolean")
	assert(type(m.diffuse) == "table")
	assert(type(m.flags) == "number")
	assert(type(m.friction) == "number")
	assert(type(m.id) == "number")
	assert(type(m.name) == "string")
	assert(type(m.shader) == "string")
	assert(type(m.shininess) == "number")
	assert(type(m.specular) == "table")
	assert(type(m.texture) == "table")
	assert(type(m.texture_scale) == "number")
	assert(type(m.type) == "string")
end)
