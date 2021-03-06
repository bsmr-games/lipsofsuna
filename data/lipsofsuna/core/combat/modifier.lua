--- Instant or effect-over-time modification of objects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.modifier
-- @alias Modifier

local Class = require("system/class")

--- Instant or effect-over-time modification of objects.
-- @type Modifier
local Modifier = Class("Modifier")

--- Creates a new modifier.
-- @param clss Modifier class.
-- @param spec Modifier spec.
-- @param object Modified object.
-- @param owner Caster object. Nil for none.
-- @param point Point in world space. Nil for object position.
-- @return Modifier.
Modifier.new = function(clss, spec, object, owner, point)
	local self = Class.new(clss)
	self.spec = spec
	self.name = spec.name
	self.object = object
	self.owner = owner
	self.point = point or self.object:get_position():copy()
	return self
end

--- Restarts the modifier with a new value.
-- @param self Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
Modifier.restart = function(self, value)
	if self.spec.restart then
		return self.spec.restart(self, value)
	else
		if value or self.strength then
			self.strength = math.max(self.strength or 0, value or 0)
		end
		return true
	end
end

--- Starts the modifier.
-- @param self Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
Modifier.start = function(self, value)
	if not self.spec.start then return end
	return self.spec.start(self, value)
end

--- Starts the modifier.
-- @param self Modifier.
-- @param value Strength of the modifier.
Modifier.start_terrain = function(self, value)
	if not self.spec.start_terrain then return end
	return self.spec.start_terrain(self, value)
end

--- Updates the modifier
-- @param self Modifier.
-- @param secs Seconds since the last update..
-- @return True to continue effect-over-time updates. False otherwise.
Modifier.update = function(self, secs)
	if not self.spec.update then return end
	return self.spec.update(self, secs)
end

--- Gets the attribute modifications caused by the modifier.
-- @param self Modifier.
-- @param attr Dictionary of attributes.
Modifier.get_attributes = function(self, attr)
	if not self.spec.attributes then return end
	self.spec.attributes(self, attr)
end

--- Gets the duration of the modifier.
-- @param self Modifier.
-- @return Duration in seconds. Nil for infinite.
Modifier.get_duration = function(self)
	if not self.spec.get_duration then return self.strength end
	return self.spec.get_duration(self)
end

return Modifier
