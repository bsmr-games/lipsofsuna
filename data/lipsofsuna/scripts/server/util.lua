-- FIXME: These need to be cleaned up.

Actions = Class()
Actions.register = function(self, args)
	self[args.name] = args.func
end

Aabb = Class()

Aabb.new = function(clss, args)
	local self = Class.new(clss, args)
	self.point = self.point or Vector()
	self.size = self.size or Vector()
	return self
end

Aabb.intersects = function(self, aabb)
	if self.point.x + self.size.x <= aabb.point.x then return end
	if self.point.y + self.size.y <= aabb.point.y then return end
	if self.point.z + self.size.z <= aabb.point.z then return end
	if self.point.x >= aabb.point.x + aabb.size.x then return end
	if self.point.y >= aabb.point.y + aabb.size.y then return end
	if self.point.z >= aabb.point.z + aabb.size.z then return end
	return true
end

Config = {}
Config.center = Vector(1024, 1024, 1024)
Config.spawn = Vector(1026, 1018, 1024)

Config.tilewidth = 32 / Voxel.tiles_per_line
Config.tilescale = 1 / Config.tilewidth
Config.gravity = Vector(0, -20, 0)
Config.skillregen = 0.5

Skills.get_total = function(self)
	local n = self:get_names()
	local t = 0
	for k,v in pairs(n) do
		t = t + self:get_maximum{skill = v}
	end
	return t
end

Damage = {}
Damage.impulse = function(self, value)
	return 0.03 * value
end

--- Handles interaction between players and terrain.
-- @param self Voxels class.
-- @param player Player object.
-- @param point Contact point.
Voxel.player_contact = function(self, player, point)
	local resist = 30
	while true do
		local t,c = Voxel:find_voxel{point = point, match = "full"}
		if not c or (c - point).length > 3 then break end
		local m = Material:find{id = t.terrain}
		if not m then break end
		if m.name == "trapdoor-000" then
			Voxel:erase{point = c}
		else
			if m.name == "spikes-000" then
				resist = 0
			end
			break
		end
	end
	return resist
end

Voxel.damage = function(self, point)
	local t = Voxel:get_tile{point = point}
	local m = Material:find{id = t}
	if not m then return end
	-- Change tile type.
	local n = Material:find{name = m.mining_transform}
	Voxel:set_tile{point = point, tile = n and n.id or 0}
	-- Create materials.
	if m.mining_materials then
		for k,v in pairs(m.mining_materials) do
			for i = 1,v do
				local offset = Vector(math.random(), math.random(), math.random())
				Item:create{name = k, position = (point + offset) * Config.tilewidth, realized = true}
			end
		end
	end
end
