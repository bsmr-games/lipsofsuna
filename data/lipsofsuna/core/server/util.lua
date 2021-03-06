--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.util
-- @alias Utils

local Aabb = require("system/math/aabb")
local Class = require("system/class")
local Vector = require("system/math/vector")

-- FIXME: These need to be cleaned up.

--- TODO:doc
-- @type Utils
Utils = Class("Utils")

--- Checks if there's room for the model in the given point in space.
-- @param clss Utils class.
-- @param point Point in world space.
-- @param model Model name.
-- @return True if there's room for the model.
Utils.check_room = function(clss, point, model)
	-- FIXME: Should rather do physics based testings.
	for k,v in pairs(Main.objects.objects_by_id) do
		local d = (v:get_position() - point).length
		if d < 1.5 then return end
	end
	return true
end

--- Finds a buildable point near the given point.
-- @param clss Utils class.
-- @param point Point in world space.
-- @param user Builder actor used for resolving conflicts.
-- @return Point in tiles and point in world space, or nil.
Utils.find_build_point = function(clss, point, user)
	-- Find an empty tile.
	local t,p = Voxel:find_tile{match = "empty", point = point}
	if not t then return end
	-- Return it if the character doesn't intersect with it.
	-- TODO: Should check for other objects as well.
	local tile = Aabb{point = p * Voxel.tile_size, size = Vector(1, 1, 1) * Voxel.tile_size}
	local char = Aabb{point = user:get_position() - Vector(0.5, 0, 0.5), size = Vector(1.5, 2, 1.5)}
	if not tile:intersects(char) then return t,p end
	-- Try to resolve a better position.
	-- If the player is standing on the tile, look for an empty tile in the
	-- direction where she's facing. This makes building bridges easier.
	local above = Voxel:get_tile(p + Vector(0,1))
	local below = Voxel:get_tile(p - Vector(0,1))
	if above ~= 0 then
		p.y = p.y + 1
	elseif below ~= 0 then
		p.y = p.y - 1
	else
		return
	end
	local dir = user:get_rotation() * Vector(0,0,-1)
	if math.abs(dir.x) > math.abs(dir.z) then
		if dir.x < 0 then
			p.x = p.x - 1
		else
			p.x = p.x + 1
		end
	else
		if dir.z < 0 then
			p.z = p.z - 1
		else
			p.z = p.z + 1
		end
	end
	t = Voxel:get_tile(p)
	if t ~= 0 then return end
	return t,p
end

--- Finds empty ground directly below the given point.
-- @param clss Utils class.
-- @param point Point in world space.
-- @return Point in world space. Nil if not found.
Utils.find_empty_ground = function(clss, point)
	local x,z = Main.terrain:get_grid_xz_by_point(point.x, point.z)
	local y = Main.terrain.terrain:find_nearest_empty_stick(x, z, point.y)
	if math.abs(y - point.y) > 5 then return end
	return Vector(point.x, y, point.z)
end

--- Finds a drop point suitable for an item.
-- @param clss Utils class.
-- @param point Point in world space.
-- @return Point in world units, or nil.
Utils.find_drop_point = function(clss, point)
	-- FIXME
	do return end
	-- Find an empty tile.
	local t,c = Voxel:find_tile{match = "empty", point = point, radius = Voxel.tile_size}
	if not t then return end
	-- TODO: Try to avoid other objects.
	return (c + Vector(0.5,0.5,0.5)) * Voxel.tile_size
end

--- Finds a random spawn point in the overworld that is suitable for actors.
-- @param clss Utils class.
-- @return Point in world units.
Utils.find_random_overworld_point = function(clss)
	-- FIXME: Stick terrain.
	local point = Vector()
	while true do
		point.x = Map.aabb.point.x + Map.aabb.size.x * math.random()
		point.z = Map.aabb.point.z + Map.aabb.size.z * math.random()
		local hm = 1710--Map.heightmap:get_height(point, false)
		if hm then
			point.y = hm
			return point
		end
	end
end

--- Finds a spawn point suitable for actors.
-- @param clss Utils class.
-- @param point Point in world space.
-- @param height Actor height. Nil for default.
-- @return Point in world units, or nil.
Utils.find_spawn_point = function(clss, point, height)
	-- Find the terrain surface.
	local x = math.floor(point.x / Main.terrain.grid_size)
	local z = math.floor(point.z / Main.terrain.grid_size)
	local y = Main.terrain.terrain:find_nearest_empty_stick(x, z, point.y, height or 2)
	-- Check that the point was valid.
	if y == 0 or y > 10000 then return end
	return Vector(point.x, y, point.z)
end

--- Finds spawns point suitable for actors.
-- @param clss Utils class.
-- @param chunk Chunk ID.
-- @param count Spawn point count.
-- @param allow_yield True to allow yielding.
-- @return List of vectors in world units.
Utils.find_spawn_points_in_sector = function(clss, chunk, count, allow_yield)
	-- FIXME: Stick terrain.
	return {}
end

--- Finds a summon point suitable for actors.
-- @param clss Utils class.
-- @param point Point in world space.
-- @return Point in world units, or nil.
Utils.find_summon_point = function(clss, point)
	-- FIXME: Stick terrain.
	return nil
end

--- Gets the default spawn point for players.
--
-- This is high in the sky because the player will be asynchrously teleported
-- to the ground level once the terrain is loaded. Before that, nothing else
-- can be done but placed the player somewhere that will not intersect with
-- the terrain.
--
-- @param self Utils class.
-- @return Vector.
Utils.get_player_spawn_point = function(self)
	return self.__player_spawn_point or Vector(5000,50000,5000)
end

Utils.set_player_spawn_point = function(self, value)
	self.__player_spawn_point = value
end

--- Gets the difficulty of the spawn point.
-- @param self Utils class.
-- @param point Point in world space.
-- @param rare True to allow rare strong monsters.
-- @return Difficulty value.
Utils.get_spawn_point_difficulty = function(self, point, rare)
	local spawn = Utils:get_player_spawn_point()
	local d = math.min(1, (point - spawn).length / 500)
	if rare then
		if math.random(1, 5) == 5 then d = d * 2 end
		if math.random(1, 5) == 5 then d = d * 2 end
	end
	return d
end

--- Spawns a plant or an item.
-- @param clss Utils class.
-- @param point Point in tiles.
-- @return True if succeeded.
Utils.spawn_plant_or_item = function(clss, point)
	-- FIXME: Stick terrain.
	return nil
end

--- Creates an explosion.
-- @param clss Utils class.
-- @param point Point in world space.
-- @param radius Radius in tiles.
-- @return True if there's room for the model.
Utils.explosion = function(clss, point, radius)
	local r1 = radius or 1
	local r2 = r1 + 3
	Main.vision:world_effect(point, "explosion1")
	-- Damage nearby objects.
	for k1,v1 in pairs(Main.objects:find_by_point(point, r2)) do
		local diff = v1:get_position() - point
		local frac = diff.length / r2
		local mult = 10 * math.min(100, v1.physics:get_mass())
		local impulse = diff:normalize() * (mult * (1 - 0.3 * frac))
		v1.physics:impulse(impulse)
		v1:damaged{amount = 40 * (1 - frac), type = "explosion"}
	end
	-- Damage terrain.
	local BuildingUtils = require("building/building-utils")
	BuildingUtils:destroy_terrain_sphere(nil, point, radius or 1.5)
end

return Utils
