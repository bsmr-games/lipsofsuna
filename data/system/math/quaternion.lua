--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.math.quaternion
-- @alias Quaternion

local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("math") then
	error("loading extension `math' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Quaternion
local Quaternion = Class("Quaternion")

--- The conjugate of the quaternion.
-- @name Quaternion.conjugate
-- @class table

--- Euler angle presentation of the quaternion.
-- @name Quaternion.euler
-- @class table

--- Length.
-- @name Quaternion.length
-- @class table

--- W value.
-- @name Quaternion.w
-- @class table

--- X value.
-- @name Quaternion.x
-- @class table

--- Y value.
-- @name Quaternion.y
-- @class table

--- Z value.
-- @name Quaternion.z
-- @class table

local quaternion_getters = {
	conjugate = function(self) return Quaternion:new_from_handle(Los.quaternion_get_conjugate(self.handle)) end,
	euler = function(self) return Los.quaternion_get_euler(self.handle) end,
	length = function(self) return Los.quaternion_get_length(self.handle) end,
	w = function(self) return Los.quaternion_get_w(self.handle) end,
	x = function(self) return Los.quaternion_get_x(self.handle) end,
	y = function(self) return Los.quaternion_get_y(self.handle) end,
	z = function(self) return Los.quaternion_get_z(self.handle) end}

local quaternion_setters = {
	length = function(self, v) Los.vector_set_length(self.handle, v) end,
	w = function(self, v) Los.quaternion_set_w(self.handle, v) end,
	x = function(self, v) Los.quaternion_set_x(self.handle, v) end,
	y = function(self, v) Los.quaternion_set_y(self.handle, v) end,
	z = function(self, v) Los.quaternion_set_z(self.handle, v) end}

local quaternion_index = function(self, k)
	local func = quaternion_getters[k]
	if func then
		return func(self)
	else
		local res = rawget(self, k)
		if res ~= nil then return res end
		local clss = rawget(self, "class")
		return clss[k]
	end
end

local quaternion_newindex = function(self, k, v)
	local func = quaternion_setters[k]
	if func then
		func(self, v)
	else
		rawset(self, k, v)
	end
end

--- Calculates the sum of two quaternions.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
local quaternion_add = function(self, quat)
	local handle = Los.quaternion_add(self.handle, quat.handle)
	return Quaternion:new_from_handle(handle)
end

--- Multiplies the quaternion with another value.<br/>
-- The second value can be a scalar, a vector, or another quaternion.
-- If it is a scalar, all the components of the quaternion are multiplied by it.
-- If it is a vector, the vector is rotated by the quaternion, and
-- if it is another quaternion, the rotations of the quaternions are concatenated.
-- @param self Quaternion.
-- @param value Quaternion, vector, or number.
-- @return New quaternion or vector.
local quaternion_mul = function(self, value)
	if type(value) == "number" then
		local handle = Los.quaternion_mul(self.handle, value)
		return Quaternion:new_from_handle(handle)
	elseif value.class == Quaternion then
		local handle = Los.quaternion_mul(self.handle, value.handle)
		return Quaternion:new_from_handle(handle)
	else
		local handle = Los.quaternion_mul(self.handle, value.handle)
		return Vector:new_from_handle(handle)
	end
end

--- Subtracts a quaternion from another.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
local quaternion_sub = function(self, quat)
	local handle = Los.quaternion_sub(self.handle, quat.handle)
	return Quaternion:new_from_handle(handle)
end

--- Converts the quaternion to a string.
-- @param self Quaternion.
-- @return String.
local quaternion_tostring = function(self)
	return Los.quaternion_tostring(self.handle)
end

--- Creates a new quaternion.<br>
--
-- If a table is passed as the first argument, the following additional
-- constuction arguments can be used:<br/>
--
-- <ul>
--   <li>angle: Angle of rotation in radians.</li>
--   <li>axis: Axis of rotation.</li>
--   <li>dir: Look direction vector.</li>
--   <li>euler: Euler angles.</li>
--   <li>up: Up direction vector.</li>
-- </ul>
--
-- @param clss Quaternion class.
-- @param x Optional X value, default is 0.
-- @param y Optional Y value, default is 0.
-- @param z Optional Z value, default is 0.
-- @param w Optional W value, default is 1.
-- @return New quaternion.
Quaternion.new = function(clss, x, y, z, w)
	local h
	if type(x) == "table" then
		if x.euler then
			h = Los.quaternion_new_euler(x.euler)
		elseif x.axis then
			h = Los.quaternion_new{axis = x.axis.handle, angle = x.angle}
		else
			h = Los.quaternion_new{dir = x.dir.handle, up = x.up.handle}
		end
	else
		h = Los.quaternion_new(x, y, z, w)
	end
	return Quaternion:new_from_handle(h)
end

--- Creates a new quaternion from an internal handle.
-- @param clss Quaternion class.
-- @param h Handle.
-- @return New quaternion.
Quaternion.new_from_handle = function(clss, h)
	local self = Class.new(clss)
	self.handle = h
	self.__index = quaternion_index
	self.__newindex = quaternion_newindex
	self.__add = quaternion_add
	self.__mul = quaternion_mul
	self.__sub = quaternion_sub
	self.__tostring = quaternion_tostring
	return self
end

--- Creates a quaternion that rotates from the source point to the destination.
-- @param clss Quaternion class.
-- @param src Vector.
-- @param dst Vector.
-- @return New quaternion.
Quaternion.new_vectors = function(clss, src, dst)
	local self = Class.new(clss)
	self.handle = Los.quaternion_new_vectors(src.handle, dst.handle)
	return self
end

--- Returns a copy of the quaternion.
-- @param self Quaternion.
-- @return New quaternion.
Quaternion.copy = function(self)
	return Quaternion(self.x, self.y, self.z, self.w)
end

--- Normalized linear interpolation.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @param blend Interpolation factor.
-- @return New quaternion.
Quaternion.nlerp = function(self, quat, blend)
	local handle = Los.quaternion_nlerp(self.handle, quat.handle, blend)
	return Quaternion:new_from_handle(handle)
end

--- Calculates the normalized form of the quaternion.
-- @param self Quaternion.
-- @return New quaternion.
Quaternion.normalize = function(self)
	local handle = Los.quaternion_normalize(self.handle)
	return Quaternion:new_from_handle(handle)
end

return Quaternion


