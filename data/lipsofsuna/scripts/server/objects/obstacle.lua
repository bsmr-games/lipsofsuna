Obstacle = Class(Object)

--- Creates an obstacle.
-- @param clss Mover class.
-- @param args Arguments.
-- @return New obstacle.
Obstacle.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	local copy = function(n, d)
		if args[n] ~= nil or d then
			self[n] = (args[n] ~= nil) and args[n] or d
		end
	end
	copy("angular")
	copy("health")
	copy("name")
	copy("rotation")
	copy("position")
	copy("spec")
	copy("realized")
	return self
end

--- Implements setters.
-- @param self Obstacle or Obstacle class.
-- @param key Key.
-- @param value Value.
Obstacle.setter = function(self, key, value)
	if key == "spec" then
		local spec = type(value) == "string" and Obstaclespec:find{name = value} or value
		if not spec then return end
		Object.setter(self, key, spec)
		self.collision_group = spec.collision_group
		self.collision_mask = spec.collision_mask
		self.model = spec.model
		self.mass = spec.mass
		self.gravity = spec.gravity
		self.physics = spec.physics
		if spec.marker then
			self.marker = Marker{name = spec.marker, position = self.position, target = self.id}
		end
	elseif key == "realized" then
		Object.setter(self, key, value)
		if value then
			for k,v in pairs(self.spec.constraints) do
				if v[1] == "hinge" then
					self:insert_hinge_constraint{position = v[2], axis = v[3]}
				end
			end
		end
	else
		Object.setter(self, key, value)
	end
end

--- Causes the obstacle to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Obstacle.damaged = function(self, amount)
	-- Obstacles whose spec has no health are indestructible.
	if not self.spec.health then return end
	-- Subtract health.
	local h = self.health or self.spec.health
	self.health = math.max(0, h - amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
	end
end

--- Hides the object and purges it from the database.
-- @param self Object to kill.
Obstacle.die = function(self)
	for k,v in ipairs(self.spec.destroy_items) do
		local spec = Itemspec:find{name = v[1]}
		if spec then
			local p = self.position + self.rotation * (v[2] or Vector())
			local r = self.rotation * (v[3] or Quaternion())
			local o = Item{random = true, spec = spec, position = p, rotation = r, realized = true}
		end
	end
	Object.die(self)
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Obstacle.use_cb = function(self, user)
	if self.spec.harvest_enabled then
		-- Create list of harvestable items.
		local mats = {}
		for k,v in pairs(self.spec.harvest_materials) do table.insert(mats, k) end
		if #mats == 0 then return end
		-- Effect playback.
		if self.spec.harvest_effect then
			Effect:play{effect = self.spec.harvest_effect, point = self.position}
		end
		-- Choose a random item from the list.
		local item = Item{spec = Itemspec:find{name = mats[math.random(1, #mats)]}}
		user:add_item{object = item}
		user:send{packet = Packet(packets.MESSAGE, "string", "Harvested " .. item.name .. ".")}
		-- Harvesting behavior.
		if self.spec.harvest_behavior == "destroy" then
			self:die()
		end
	end
	Object.use_cb(self, user)
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Obstacle.write = function(self)
	return string.format("local self=Obstacle%s\n%s", serialize{
		angular = self.angular,
		health = self.health,
		id = self.id,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec.name},
		"return self")
end
