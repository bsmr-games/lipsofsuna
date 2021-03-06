--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.lighting
-- @alias Lighting

local Class = require("system/class")
local Light = require("system/light")
local Quaternion = require("system/math/quaternion")
local Render = require("system/render")

--- TODO:doc
-- @type Lighting
local Lighting = Class("Lighting")

Lighting.new = function(clss)
	local self = Class.new(clss)
	self.light_global = Light()
	self.light_global:set_diffuse{1,1,1,1}
	self.light_global:set_directional(true)
	self.light_global:set_equation{1.9,0,0}
	self.light_global:set_shadow_casting(true)
	self.light_spell = Light()
	self.light_spell:set_diffuse{1,1,1,1}
	self.light_spell:set_equation{1.5,0,0.05}
	self.light_spell_timer = 0
	Render:set_scene_ambient{0.5,0.5,0.5}
	-- Set the skybox.
	Render:set_skybox("skybox1")
	return self
end

Lighting.update = function(self, secs)
	-- Update the global light.
	if self.dungeon_mode then
		self.light_global:set_rotation(Quaternion(-0.68,0,0,0.82))
	else
		self.light_global:set_rotation(Quaternion(-0.16,-0.84,-0.32,0.42))
	end
	self.light_global:set_enabled(true)
	-- Update the light spell position.
	if Client.player_object and self.light_spell:get_enabled() then
		local p = Client.player_object:get_position()
		self.light_spell_timer = self.light_spell_timer + secs
		p.x = p.x + math.sin(self.light_spell_timer)
		p.y = p.y + math.cos(0.9*self.light_spell_timer)
		p.z = p.z + math.cos(-0.8*self.light_spell_timer)
		local r = Client.player_object:get_rotation()
		self.light_spell:set_position(p + r * Vector(0,1.7,-1.5))
	end
end

--- Changes the lighting to be overworld or dungeon-like.
-- @param self Lighting class.
-- @param value Boolean, true for dungeon, false for overworld.
Lighting.set_dungeon_mode = function(self, value)
	-- Update the mode flag.
	if self.dungeon_mode == value then return end
	self.dungeon_mode = value
	-- Reconfigure the brightness of the global light.
	if value then
		self.light_global:set_equation{6,0,0}
		Render:set_scene_ambient{0.3,0.3,0.3}
	else
		self.light_global:set_equation{1.9,0,0}
		Render:set_scene_ambient{0.5,0.5,0.5}
	end
	-- Reconfigure the visibility of the skybox.
	Render:set_skybox(value and "" or "skybox1")
end

Lighting.set_light_spell = function(self, value)
	self.light_spell:set_enabled(value)
end

return Lighting


