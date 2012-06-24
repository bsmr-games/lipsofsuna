require "system/class"

if not Los.program_load_extension("math") then
	error("loading extension `math' failed")
end

------------------------------------------------------------------------------

Aabb = Class()
Aabb.class_name = "Aabb"

--- Creates a new axis-aligned bounding box.
-- @param clss Aabb class.
-- @param args Arguments.<ul>
--   <li>point: Minimum point vector.</li>
--   <li>size: Size vector.</li></ul>
-- @return Aabb.
Aabb.new = function(clss, args)
	local self = Class.new(clss, args)
	self.point = self.point or Vector()
	self.size = self.size or Vector()
	return self
end

--- Returns the distance to the given point.
-- @param self Aabb.
-- @param point Vector.
-- @return Number.
Aabb.get_distance_to_point = function(self, point)
	return (point - self:get_nearest_point(point)).length
end

--- Returns the point inside the box closest to the given point.
-- @param self Aabb.
-- @param point Vector.
-- @return Vector.
Aabb.get_nearest_point = function(self, point)
	return Vector(
		math.min(math.max(point.x, self.point.x), self.point.x + self.size.x),
		math.min(math.max(point.y, self.point.y), self.point.y + self.size.y),
		math.min(math.max(point.z, self.point.z), self.point.z + self.size.z))
end

--- Return true if the AABB intersects with another AABB.
-- @param self Aabb.
-- @param aabb Aabb.
-- @return True if intersects.
Aabb.intersects = function(self, aabb)
	if self.point.x + self.size.x <= aabb.point.x then return end
	if self.point.y + self.size.y <= aabb.point.y then return end
	if self.point.z + self.size.z <= aabb.point.z then return end
	if self.point.x >= aabb.point.x + aabb.size.x then return end
	if self.point.y >= aabb.point.y + aabb.size.y then return end
	if self.point.z >= aabb.point.z + aabb.size.z then return end
	return true
end

--- Return true if the AABB intersects with a point.
-- @param self Aabb.
-- @param point Position vector.
-- @return True if intersects.
Aabb.intersects_point = function(self, point)
	if point.x < self.point.x then return end
	if point.y < self.point.y then return end
	if point.z < self.point.z then return end
	if point.x > self.point.x + self.size.x then return end
	if point.y > self.point.y + self.size.x then return end
	if point.z > self.point.z + self.size.x then return end
	return true
end

------------------------------------------------------------------------------

Quaternion = Class()
Quaternion.class_name = "Quaternion"

--- Creates a new quaternion.
-- @param clss Quaternion class.
-- @param args Arguments.<ul>
--   <li>1: X coordinate.</li>
--   <li>2: Y coordinate.</li>
--   <li>3: Z coordinate.</li>
--   <li>4: W coordinate.</li></ul>
-- @return New quaternion.

--- Creates a new quaternion.
-- @param clss Quaternion class.
-- @param args Arguments.<ul>
--   <li>1,x: Optional X value, default is 0.</li>
--   <li>2,y: Optional Y value, default is 0.</li>
--   <li>3,z: Optional Z value, default is 0.</li>
--   <li>4,w: Optional W value, default is 1.</li>
--   <li>angle: Angle of rotation in radians.</li>
--   <li>axis: Axis of rotation.</li>
--   <li>dir: Look direction vector.</li>
--   <li>euler: Euler angles.</li>
--   <li>up: Up direction vector.</li></ul>
-- @return New quaternion.
Quaternion.new = function(clss, x, y, z, w)
	local self = Class.new(clss)
	if type(x) == "table" then
		if x.euler then
			self.handle = Los.quaternion_new_euler(x.euler)
		elseif x.axis then
			self.handle = Los.quaternion_new{axis = x.axis.handle, angle = x.angle}
		else
			self.handle = Los.quaternion_new{dir = x.dir.handle, up = x.up.handle}
		end
	else
		self.handle = Los.quaternion_new(x, y, z, w)
	end
	__userdata_lookup[self.handle] = self
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
	__userdata_lookup[self.handle] = self
	return self
end

--- Returns a copy of the quaternion.
-- @param self Quaternion.
-- @return New quaternion.
Quaternion.copy = function(self)
	return Quaternion(self.x, self.y, self.z, self.w)
end

--- Calculates the sum of two quaternions.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
Quaternion.__add = function(self, quat)
	local handle = Los.quaternion_add(self.handle, quat.handle)
	return Class.new(Quaternion, {handle = handle})
end

--- Multiplies the quaternion with another value.<br/>
-- The second value can be a scalar, a vector, or another quaternion.
-- If it is a scalar, all the components of the quaternion are multiplied by it.
-- If it is a vector, the vector is rotated by the quaternion, and
-- if it is another quaternion, the rotations of the quaternions are concatenated.
-- @param self Quaternion.
-- @param value Quaternion, vector, or number.
-- @return New quaternion or vector.
Quaternion.__mul = function(self, value)
	if type(value) == "number" then
		local handle = Los.quaternion_mul(self.handle, value)
		return Class.new(Quaternion, {handle = handle})
	elseif value.class == Quaternion then
		local handle = Los.quaternion_mul(self.handle, value.handle)
		return Class.new(Quaternion, {handle = handle})
	else
		local handle = Los.quaternion_mul(self.handle, value.handle)
		return Class.new(Vector, {handle = handle})
	end
end

--- Subtracts a quaternion from another.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
Quaternion.__sub = function(self, quat)
	local handle = Los.quaternion_sub(self.handle, quat.handle)
	return Class.new(Quaternion, {handle = handle})
end

--- Converts the quaternion to a string.
-- @param self Quaternion.
-- @return String.
Quaternion.__str = function(self)
	return Los.quaternion_tostring(self.handle)
end

--- Normalized linear interpolation.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @param blend Interpolation factor.
-- @return New quaternion.
Quaternion.nlerp = function(self, quat, blend)
	local handle = Los.quaternion_nlerp(self.handle, quat.handle, blend)
	return Class.new(Quaternion, {handle = handle})
end

--- Calculates the normalized form of the quaternion.
-- @param self Quaternion.
-- @return New quaternion.
Quaternion.normalize = function(self)
	local handle = Los.quaternion_normalize(self.handle)
	return Class.new(Quaternion, {handle = handle})
end

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

Quaternion:add_getters{
	conjugate = function(s) return Class.new(Quaternion, {handle = Los.quaternion_get_conjugate(s.handle)}) end,
	euler = function(s) return Los.quaternion_get_euler(s.handle) end,
	length = function(s) return Los.quaternion_get_length(s.handle) end,
	w = function(s) return Los.quaternion_get_w(s.handle) end,
	x = function(s) return Los.quaternion_get_x(s.handle) end,
	y = function(s) return Los.quaternion_get_y(s.handle) end,
	z = function(s) return Los.quaternion_get_z(s.handle) end}

Quaternion:add_setters{
	length = function(s, v) Los.vector_set_length(s.handle, v) end,
	w = function(s, v) Los.quaternion_set_w(s.handle, v) end,
	x = function(s, v) Los.quaternion_set_x(s.handle, v) end,
	y = function(s, v) Los.quaternion_set_y(s.handle, v) end,
	z = function(s, v) Los.quaternion_set_z(s.handle, v) end}

------------------------------------------------------------------------------

Vector = Class()
Vector.class_name = "Vector"

--- Creates a new vector.
-- @param clss Vector class.
-- @param args Arguments.<ul>
--   <li>1: X coordinate.</li>
--   <li>2: Y coordinate.</li>
--   <li>3: Z coordinate.</li></ul>
-- @return New vector.
Vector.new = function(clss, x, y, z)
	local self = Class.new(clss)
	self.handle = Los.vector_new()
	__userdata_lookup[self.handle] = self
	Los.vector_set_x(self.handle, x)
	Los.vector_set_y(self.handle, y)
	Los.vector_set_z(self.handle, z)
	return self
end

--- Calculates the absolute of the vector in-place.
-- @param self Vector.
-- @return New vector.
Vector.abs = function(self)
	self.x = math.abs(self.x)
	self.y = math.abs(self.y)
	self.z = math.abs(self.z)
	return self
end

--- Add a vector to another in-place.
-- @param self Vector.
-- @param vector Vector.
-- @return Self.
Vector.add = function(self, v)
	Los.vector_add(self.handle, v.handle)
	return self
end

--- Add components to the vector in-place.
-- @param self Vector.
-- @param x X component.
-- @param y Y component.
-- @param z Z component.
-- @return Self.
Vector.add_xyz = function(self, x, y, z)
	if x then self.x = self.x + x end
	if y then self.y = self.y + y end
	if z then self.z = self.z + z end
	return self
end

--- Calculates the ceil of the vector in-place.
-- @param self Vector.
-- @return Self.
Vector.ceil = function(self)
	self.x = math.ceil(self.x)
	self.y = math.ceil(self.y)
	self.z = math.ceil(self.z)
	return self
end

--- Returns a copy of the vector.
-- @param self Vector.
-- @return New vector.
Vector.copy = function(self)
	return Vector(self.x, self.y, self.z)
end

--- Divides the vector by a scalar in-place.
-- @param self Vector.
-- @param scalar Scalar.
-- @return Self.
Vector.divide = function(self, value)
	if value ~= 0 then
		Los.vector_mul(self.handle, 1/value)
	end
	return self
end

--- Calculates the floor of the vector in-place.
-- @param self Vector.
-- @return Self.
Vector.floor = function(self)
	self.x = math.floor(self.x)
	self.y = math.floor(self.y)
	self.z = math.floor(self.z)
	return self
end

--- Calculates the cross product of two vectors in-place.
-- @param self Vector.
-- @param v Vector.
-- @return Self.
Vector.cross = function(self, v)
	Los.vector_cross(self.handle, v.handle)
	return self
end

--- Calculates the dot product of two vectors.
-- @param self Vector.
-- @param v Vector.
-- @return Scalar.
Vector.dot = function(self, v)
	return Los.vector_dot(self.handle, v.handle)
end

--- Interpolates two vectors linearly in-place.
-- @param self Vector.
-- @param v Vector.
-- @param f Scalar in range [0, 1], where 0 corresponds to the own value.
Vector.lerp = function(self, v, f)
	self.x = self.x * (1 - f) + v.x * f
	self.y = self.y * (1 - f) + v.y * f
	self.z = self.z * (1 - f) + v.z * f
end

--- Multiplies the vector by a scalar in-place.
-- @param self Vector.
-- @param scalar Scalar.
-- @return Self.
Vector.multiply = function(self, value)
	Los.vector_mul(self.handle, value)
	return self
end

--- Normalizes the vector in-place.
-- @param self Vector.
-- @return Self.
Vector.normalize = function(self)
	Los.vector_normalize(self.handle)
	return self
end

--- Calculates the vector rounded to the nearest integers in-place.
-- @param self Vector.
-- @return Self.
Vector.round = function(self)
	self.x = math.ceil(self.x + 0.5)
	self.y = math.ceil(self.y + 0.5)
	self.z = math.ceil(self.z + 0.5)
	return self
end

--- Set the components of the vector in-place.
-- @param self Vector.
-- @param x X component.
-- @param y Y component.
-- @param z Z component.
-- @return Self.
Vector.set_xyz = function(self, x, y, z)
	if x then self.x = x end
	if y then self.y = y end
	if z then self.z = z end
	return self
end

--- Subtracts a vector from another in-place.
-- @param self Vector.
-- @param vector Vector.
-- @return Self.
Vector.subtract = function(self, v)
	Los.vector_sub(self.handle, v.handle)
	return self
end

--- Substracts components from the vector in-place.
-- @param self Vector.
-- @param x X component.
-- @param y Y component.
-- @param z Z component.
-- @return Self.
Vector.subtract_xyz = function(self, x, y, z)
	if x then self.x = self.x - x end
	if y then self.y = self.y - y end
	if z then self.z = self.z - z end
	return self
end

--- Multiplies the vector by a quaternion in-place.
-- @param self Vector.
-- @param q Quaternion.
-- @return Self.
Vector.transform = function(self, q)
	Los.quaternion_mul(q.handle, self.handle)
	return self
end

--- Calculates the sum of two vectors.
-- @param self Vector.
-- @param v Vector.
-- @return New vector.
Vector.__add = function(self, v)
	local copy = self:copy()
	Los.vector_add(copy.handle, v.handle)
	return copy
end

--- Multiplies the vector by a scalar or quaternion.
-- @param self Vector.
-- @param scalar Scalar.
-- @return New vector.
Vector.__mul = function(self, value)
	if type(value) == "number" then
		local copy = self:copy()
		Los.vector_mul(copy.handle, value)
		return copy
	elseif value.class == Quaternion then
		local copy = self:copy()
		Los.quaternion_mul(value.handle, copy.handle)
		return copy
	end
end

--- Converts the vector to a string.
-- @param self Vector.
-- @return String.
Vector.__str = function(self)
	return Los.vector_tostring(self.handle)
end

--- Subtracts a vector from another.
-- @param self Vector.
-- @param vector Vector.
-- @return New vector.
Vector.__sub = function(self, v)
	local copy = self:copy()
	Los.vector_sub(copy.handle, v.handle)
	return copy
end

--- Length.
-- @name Vector.length
-- @class table

--- X value.
-- @name Vector.x
-- @class table

--- Y value.
-- @name Vector.y
-- @class table

--- Z value.
-- @name Vector.z
-- @class table

Vector:add_getters{
	length = function(s) return Los.vector_get_length(s.handle) end,
	x = function(s) return Los.vector_get_x(s.handle) end,
	y = function(s) return Los.vector_get_y(s.handle) end,
	z = function(s) return Los.vector_get_z(s.handle) end}

Vector:add_setters{
	length = function(s, v) Los.vector_set_length(s.handle, v) end,
	x = function(s, v) Los.vector_set_x(s.handle, v) end,
	y = function(s, v) Los.vector_set_y(s.handle, v) end,
	z = function(s, v) Los.vector_set_z(s.handle, v) end}
