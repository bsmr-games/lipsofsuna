--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.missilespell
-- @alias MissileSpell

local Class = require("system/class")
local Spell = require("core/objects/spell")

--- TODO:doc
-- @type MissileSpell
local MissileSpell = Class("MissileSpell", Spell)

--- Creates a new missile spell.
-- @param clss Missile spell class.
-- @param manager Object manager.
-- @param args Arguments.<ul>
--   <li>feat: Feat invoking the spell.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>spec: Spell spec.</li></ul>
-- @return Spell.
MissileSpell.new = function(clss, manager, args)
	local self = Spell.new(clss, manager, {feat = args.feat, owner = args.owner, spec = args.spec})
	self.speed = 3
	return self
end

--- Updates the object.
-- @param self Object.
-- @param secs Seconds since the last update.
MissileSpell.update_server = function(self, secs)
	-- Adjust rotation.
	-- Controlling is done by copying the rotation from the caster.
	self:set_rotation(self.owner:get_rotation() * self.owner.tilt)
	self.orig_rotation = self:get_rotation()
	-- Adjust velocity.
	-- Velocity is smoothed but approaches the target value quickly.
	local vel = self:get_velocity()
	self:set_velocity(Vector(0,0,-self.speed):transform(self:get_rotation(), vel):multiply(0.5))
	self.orig_velocity = vel
	-- Update the base class.
	Spell.update_server(self, secs)
end

return MissileSpell


