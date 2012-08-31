--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.boomerang_controller
-- @alias BoomerangController

local Class = require("system/class")
local Combat = require("core/server/combat")

--- TODO:doc
-- @type BoomerangController
local BoomerangController = Class("BoomerangController")

BoomerangController.new = function(clss, attacker, projectile, damage)
	local self = Class.new(clss)
	self.attacker = attacker
	self.projectile = projectile
	self.damage = damage
	return self
end

BoomerangController.attach = function(self)
	-- Set the boomerang controls.
	self.state = 0
	self.timer = 0
	self.projectile.physics:set_gravity(self.projectile.spec.gravity_projectile)
	self.projectile.controller = self
	-- Add the projectile to the world.
	local src,dst = self.attacker:get_attack_ray()
	self.projectile:detach()
	self.projectile:set_position(src + self.attacker:get_rotation() * Vector(0,0,-1))
	self.projectile:set_rotation(Quaternion{axis = Vector(0,0,1), angle = -0.5 * math.pi})
	self.projectile:set_velocity(dst:subtract(src):normalize():multiply(10))
	self.projectile:set_visible(true)
	-- Enable contact events.
	self.projectile.physics:set_contact_events(true)
	-- Animation the boomerang.
	self.projectile:animate("fly start")
end

BoomerangController.detach = function(self)
	self.projectile.physics:set_contact_events(false)
	self.projectile.controller = nil
	self.projectile.physics:set_gravity(self.projectile.spec.gravity)
	self.projectile:animate("fly stop")
end

BoomerangController.handle_contact = function(self, result)
	-- Owner catch.
	if result.object == self.attacker and not self.attacker.dead then
		if self.attacker.inventory:merge_object(self.projectile) then
			local o = self.attacker:get_weapon()
			if not o then self.attacker:set_weapon(self.projectile) end
		end
		self:detach()
		return
	end
	-- Apply the damage.
	Combat:apply_ranged_impact(self.attacker, self.projectile, self.damage, result.point, result.object, result.tile)
	-- Disable the boomerang mode.
	self:detach()
end

BoomerangController.update = function(self, secs)
	-- Wait for a state change.
	self.timer = self.timer + secs
	if self.timer < 1 then return end
	self.timer = self.timer - 1
	-- Change the state.
	if self.state == 0 then
		self.projectile:set_velocity(self.projectile:get_velocity():multiply(-2))
		self.state = 1
	elseif self.state == 1 then
		self.projectile:set_velocity(self.projectile:get_velocity():multiply(2))
		self.state = 2
	else
		self:detach()
	end
end

return BoomerangController


