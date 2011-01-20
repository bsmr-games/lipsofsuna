Item = Class(Object)
Item.pickable = true

Item.setter = function(self, key, value)
	if key == "itemspec" then
		local spec = type(value) == "string" and Itemspec:find{name = value} or value
		if not spec then return end
		Object.setter(self, key, spec)
		self.mass = spec.mass
		self.model = spec.model
		self.name = spec.name
	elseif key == "realized" then
		-- Avoid objects getting stuck inside walls.
		-- When object is created, we start a timer that periodically checks
		-- if the object is stuck until it isn't or there's no way to fix it.
		if value == self.realized then return end
		if value then
			self.stuck_timer = Timer{delay = 0.5, func = function(timer)
				timer.counter = (timer.counter or 0) + 1
				if not self:stuck_check() or (not self.stuck and timer.counter > 10) then
					timer:disable()
					self.stuck_timer = nil
				end
			end}
		elseif self.stuck_timer then
			self.stuck_timer:disable()
			self.stuck_timer = nil
		end
		Object.setter(self, key, value)
	else
		Object.setter(self, key, value)
	end
end

--- Creates a copy of the item.
-- @param self Object.
-- @return Object.
Item.clone = function(self)
	return Item{
		itemspec = self.itemspec,
		angular = self.angular,
		position = self.position,
		rotation = self.rotation}
end

--- Causes the item to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Item.damaged = function(self, amount)
	-- Items whose spec has no health are indestructible.
	if not self.itemspec.health then return end
	-- Subtract health.
	local h = self.health or self.itemspec.health
	self.health = math.max(0, h - amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
	end
end

--- Destroys the item.
-- @param self Object.
Item.die = function(self)
	-- Mark as dead.
	if self.dead then return end
	self.dead = true
	-- Execute destruction actions.
	for k,v in ipairs(self.itemspec.destroy_actions) do
		if v == "explode" then
			for k,v in pairs(Object:find{point = self.position, radius = 5}) do
				Particles:create(self.position, "explosion1")
				v:damaged(40)
			end
		end
	end
	-- Remove from the world.
	Object.die(self)
end

--- Splits items from the stack.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Number of items to split.</li></ul>
-- @return Object.
Item.split = function(self, args)
	local c = args and args.count or 1
	if c < self:get_count() then
		local o = self:clone()
		self:subtract{count = c}
		o.count = c
		return o
	end
	return self
end

--- Creates a random or a named item.
-- @param clss Item class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li>
--   <li>name: Item name.</li></ul>
-- @return Object or nil.
Item.create = function(clss, args)
	local self
	-- Select item type.
	local spec = args and args.name and Itemspec:find(args) or Itemspec:random(args)
 	if not spec then return end
	-- Create new object.
	self = Item:new(args)
	-- TODO
	self.category = nil
	self.itemspec = spec
	return self
end

--- Creates an item.
-- @param clss Item class.
-- @param args Arguments.
-- @return New item.
Item.new = function(clss, args)
	local self = Object.new(clss, args)
	self.physics = args and args.physics or "rigid"
	return self
end

--- Fires or throws the item.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile. (required)</li>
--   <li>point: Firing point relative to the owner.</li>
--   <li>speed: Initial speed.</li>
--   <li>timer: Trigger at timeout.</li></ul>
-- @return The split and fired item.
Item.fire = function(self, args)
	-- Split a projectile from the stack and fire it.
	if not args.owner or not args.feat then return end
	local proj = self:split()
	Object.fire(proj, args)
	-- Special handling for boomerangs.
	if proj.itemspec.categories["boomerang"] then
		-- Enable boomerange collisions.
		-- The boomerang mode is disabled when a collision occurs and either the
		-- collision object is damaged or the user catches the boomerang.
		if args.collision then
			proj.contact_cb = function(_, result)
				if result.object == proj.owner then
					-- Owner catch.
					local o = proj.owner:get_item{slot = "hand.R"}
					if not o then
						proj.owner:set_item{slot = "hand.R", object = proj}
					elseif not o:merge{object = proj} then
						proj.owner:add_item{object = proj}
					end
				else
					-- Damage target.
					args.feat:apply{
						attacker = args.user,
						point = result.point,
						projectile = proj,
						target = result.object,
						tile = result.tile}
				end
				-- Disable boomerang mode.
				proj.timer:disable()
				proj.gravity = Config.gravity
				proj:animate{channel = 2}
				proj.contact_cb = nil
			end
		end
		-- Enable boomerang rotation.
		if proj.itemspec.categories["boomerang"] then
			proj.rotated = 0
			proj.rotation = Quaternion{axis = Vector(0,0,1), angle = -0.5 * math.pi}
			proj:animate{animation = "fly", channel = 2, permanent = true}
			proj.gravity = Vector(0,2,0)
			proj.timer = Timer{delay = 0, func = function(self, secs)
				-- Adjust velocity vector.
				local m = 1.55 * math.pi
				local r = math.min(secs * 1.3 * math.pi, m - proj.rotated)
				proj.velocity = Quaternion{axis = Vector(0,1,0), angle = r} * proj.velocity
				-- Stop after a while.
				proj.rotated = proj.rotated + r
				if proj.rotated >= m then
					self:disable()
					proj.gravity = Config.gravity
				end
			end}
		end
	end
	-- Return the fired projectile.
	return proj
end

--- Called when the object is examined.
-- @param self Object.
-- @param user User.
Item.examine_cb = function(self, user)
	user:send{packet = Packet(packets.MESSAGE, "string", self.itemspec.name)}
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Item.use_cb = function(self, user)
	if self.itemspec.categories["tool"] then
		Crafting:send{user = user}
	elseif self.itemspec.categories["book"] then
		user:send{packet = Packet(packets.BOOK,
			"string", self.itemspec.name,
			"string", self.itemspec.book_text)}
	elseif self.itemspec.categories["potion"] then
		local types =
		{
			["lesser health potion"] = { skill = "health", value = 10 },
			["health potion"] = { skill = "health", value = 30 },
			["greater health potion"] = { skill = "health", value = 60 },
			["lesser mana potion"] = { skill = "willpower", value = 10 },
			["mana potion"] = { skill = "willpower", value = 30 },
			["greater mana potion"] = { skill = "willpower", value = 60 }
		}
		local type = types[self.name]
		if not type then return end
		local skills = user.skills
		if not skills then return end
		local value = skills:get_value{skill = type.skill}
		if not value then return end
		skills:set_value{skill = type.skill, value = value + type.value}
		Effect:play{effect = "impact1", object = user}
		self:subtract{count = 1}
	end
end

--- Called when the item is being equipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.equipped = function(self, user, slot)
	if self.itemspec.categories["shield"] then
		user:animate{animation = "hold-left", channel = Animation.CHANNEL_EQUIP_LEFT, weight = 10.0, permanent = true}
	end
end

--- Called when the item is being unequipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.unequipped = function(self, user, slot)
	if self.itemspec.categories["shield"] then
		user:animate{channel = Animation.CHANNEL_EQUIP_LEFT}
	end
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Item.write = function(self)
	return "local self=Item{" ..
		"angular=" .. serialize_value(self.angular) .. "," ..
		"id=" .. serialize_value(self.id) .. "," ..
		"itemspec=" .. serialize_value(self.itemspec.name) .. "," ..
		"position=" .. serialize_value(self.position) .. "," ..
		"rotation=" .. serialize_value(self.rotation) .. "}\n" ..
		"return self"
end
