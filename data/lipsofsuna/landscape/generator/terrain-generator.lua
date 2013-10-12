--- Procedural terrain generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.terrain_generator
-- @alias TerrainGenerator

local Class = require("system/class")
local MapUtils = require("core/server/map-utils")
local Noise = require("system/noise")
local TerrainMaterialSpec = require("core/specs/terrain-material")
local Vector = require("system/math/vector")

--- Procedural terrain generator.
-- @type TerrainGenerator
local TerrainGenerator = Class("TerrainGenerator")

--- Creates a new terrain generator.
-- @param clss TerrainGenerator class.
-- @return TerrainGenerator.
TerrainGenerator.new = function(clss)
	local self = Class.new(clss)
	self.seed1 = math.random(100, 600)
	self.seed2 = math.random(100, 600)
	self.seed3 = math.random(100, 600)
	self.seed4 = math.random(100, 600)
	self.seed5 = math.random(100, 600)
	self.seed6 = math.random(100, 600)
	self.seed7 = math.random(100, 600)
	self.seed8 = math.random(100, 600)
	self.seed9 = math.random(100, 600)
	self.seed10 = math.random(100, 600)
	return self
end

--- Generates a terrain chunk.
-- @param self TerrainGenerator.
-- @param chunk TerrainChunk.
TerrainGenerator.generate = function(self, chunk)
	local get_surface_height_at = function(x, z)
		-- Choose the bumpiness of the region.
		-- p=0.7: Very smooth.
		-- p=0.9: Very bumpy.
		local r = Noise:plasma_noise_2d(self.seed1 + 0.01 * x, self.seed2 + 0.01 * z, 2)
		local p = 0.75 + 0.15 * r
		-- Choose the height of the region.
		-- This is affected by both the position and the bumpiness.
		local n1 = Noise:harmonic_noise_2d(self.seed1 + 0.001 * x, self.seed2 + 0.001 * z, 6, 1.3, p)
		-- Choose the soil layer height.
		-- This is affected by the height and the bumpiness.
		local s_base = 0.3 - 0.7 * (r + n1)
		local s_rand = Noise:harmonic_noise_2d(self.seed3 + 0.02 * x, self.seed4 + 0.02 * z, 3, 1.3, 1 - 0.25 * (n1 + r))
		local n2 = math.max(0, 0.5 * (s_base + s_rand))
		-- Choose the grass layer height.
		-- This is mostly just random on any areas with soil.
		local g = Noise:plasma_noise_2d(self.seed1 + 0.03 * x, self.seed2 + 0.03 * z, 3 - r)
		local n3 = math.max(0, g) * n2
		-- Return the combined heights.
		return 100 + 100 * n1, 2 * n2, 0.5 * n3
	end
	local get_dungeon_height_at = function(x, z)
		-- Choose the dungeon amount of the region.
		local r = Noise:plasma_noise_2d(self.seed1 + 0.001 * x, self.seed1 + 0.001 * z, 3)
		if r < 0 then return 0, 0 end
		-- Choose the roughness of the dungeon.
		local r = Noise:plasma_noise_2d(self.seed1 + 0.01 * x, self.seed1 + 0.01 * z, 2)
		local p = 0.5 + 0.35 * r
		-- Choose the vertical offset.
		local y1 = math.abs(Noise:harmonic_noise_2d(self.seed1 + 0.001 * x, self.seed1 + 0.001 * z, 6, 1.3, p))
		local y2 = math.abs(Noise:harmonic_noise_2d(self.seed1 + 0.0001 * x, self.seed1 + 0.0001 * z, 6, 1.3, 0.5))
		-- Choose the height.
		local h = math.abs(Noise:plasma_noise_2d(self.seed1 + 0.035 * x, self.seed1 + 0.035 * z, 2))
		if h < 0.15 then
			h = 0
		end
		-- Return the combined heights,
		return 80 + 70 * y1 - 70 * y2, h * 10
	end
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	-- Calculate the heights.
	local h = {}
	local i = 0
	for z = 0,w do
		for x = 0,w do
			h[i], h[i + 1], h[i + 2] = get_surface_height_at(chunk.x + x, chunk.z + z)
			h[i + 3], h[i + 4] = get_dungeon_height_at(chunk.x + x, chunk.z + z)
			i = i + 5
		end
	end
	-- Randomize civilization obstacles.
	local civ_x,civ_y,civ_z
	if math.random() < 0.05 then
		civ_x = math.random(0, w-1)
		civ_z = math.random(0, w-1)
	end
	-- Generate the surface.
	local p = Vector()
	local stride = 5 * (w + 1)
	local get_height_at = function(x, z)
		local i = 5 * x + stride * z
		return h[i], h[i + 1], h[i + 2]
	end
	local get_dungeon_at = function(x, z)
		local i = 5 * x + stride * z
		local y, h = h[i + 3], h[i + 4]
		--return y - h*0.1, y + h*0.9
		return y - h/2, y + h/2
	end
	for z = 0,w-1 do
		for x = 0,w-1 do
			-- Calculate the corner heights.
			local a00,b00,c00 = get_height_at(x, z)
			local a10,b10,c10 = get_height_at(x + 1, z)
			local a01,b01,c01 = get_height_at(x, z + 1)
			local a11,b11,c11 = get_height_at(x + 1, z + 1)
			-- Calculate the dungeon corner heights.
			local d00,e00 = get_dungeon_at(x, z)
			local d10,e10 = get_dungeon_at(x + 1, z)
			local d01,e01 = get_dungeon_at(x, z + 1)
			local d11,e11 = get_dungeon_at(x + 1, z + 1)
			-- Sample the civilization obstacle position.
			if civ_x == x and civ_z == z then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				civ_y = (y0 + y1 + y2 + y3) / 4
			-- Generate the grass.
			elseif c00 > 0 or c10 > 0 or c01 > 0 or c11 > 0 then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				t:add_stick_corners(chunk.x + x, chunk.z + z, 0, 0, 0, 0, y0, y1, y2, y3, 2)
				-- Generate plants.
				if e00 < y0 and e10 < y1 and e01 < y2 and e11 < y3 then
					local r = math.random() - 0.99
					if r >= 0 then
						-- Calculate the position.
						p:set_xyz(chunk.x + x + 0.5, 0.0, chunk.z + z + 0.5)
						p:multiply(chunk.manager.grid_size)
						p:add_xyz(0, (y0 + y1 + y2 + y3) / 4, 0)
						-- Calculate the forest ratio.
						local f = Noise:plasma_noise_2d(self.seed5 + 0.005 * x, self.seed6 + 0.005 * z, 2)
						-- Choose and create the obstacle.
						if r > f * 0.01 then
							MapUtils:place_obstacle{point = p, category = "tree", rotation = math.random() * math.pi * 2}
						else
							MapUtils:place_obstacle{point = p, category = "small-plant", rotation = math.random() * math.pi * 2}
						end
					end
				end
			end
			-- Generate the soil.
			if b00 > 0 or b10 > 0 or b01 > 0 or b11 > 0 then
				t:add_stick_corners(chunk.x + x, chunk.z + z, 0, 0, 0, 0, a00 + b00, a10 + b10, a01 + b01, a11 + b11, 1)
			end
			-- Generate the stone.
			t:add_stick_corners(chunk.x + x, chunk.z + z, 0, 0, 0, 0, a00, a10, a01, a11, 3)
			-- Generate ore.
			local ore = math.random() * 300
			if ore < 3 and ore > 0.1 then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				local mat
				if math.random() < 0.75 then
					mat = TerrainMaterialSpec:random{category = "common ore"}
				else
					mat = TerrainMaterialSpec:random{category = "rare ore"}
				end
				t:add_stick_corners(chunk.x + x, chunk.z + z, y0 - ore, y1 - ore, y2 - ore, y3 - ore, y0, y1, y2, y3, mat.id)
			end
			-- Generate the dungeon.
			--if d00 < e00 and d10 < e10 and d01 < e01 and d11 < e11 then
			--	t:add_stick_corners(chunk.x + x, chunk.z + z, d00, d10, d01, d11, e00, e10, e01, e11, 0)
			--end
		end
	end
	-- Generate dungeons.
	local dt,dy1,dy2,dy3,dy4,dh1,dh2,dh3,dh4 = self:get_dungeon_type(chunk.manager, chunk.x, chunk.z)
	if dt then
		-- Calculate smooth floor vertices.
		local dt1,dt2,dt3,dt4 = dy1+dh1,dy2+dh2,dy3+dh3,dy4+dh4
		local bot = {}
		local top = {}
		local s = w + 1
		for z = 0,w do
			local az = z/w
			local bl = dy3 * (1-az) + dy1 * az
			local br = dy4 * (1-az) + dy2 * az
			local tl = dt3 * (1-az) + dt1 * az
			local tr = dt4 * (1-az) + dt2 * az
			for x = 0,w do
				local ax = x/w
				bot[x + z * s] = bl * (1-ax) + br * ax
				top[x + z * s] = tl * (1-ax) + tr * ax
			end
		end
		-- Generate the floor.
		for z = 0,w-1 do
			for x = 0,w-1 do
				t:add_stick_corners(chunk.x + x, chunk.z + z,
					bot[x +  z    * s], bot[(x+1) +  z    * s],
					bot[x + (z+1) * s], bot[(x+1) + (z+1) * s],
					top[x +  z    * s], top[(x+1) +  z    * s],
					top[x + (z+1) * s], top[(x+1) + (z+1) * s],
					0)
			end
		end
	end
	-- Smoothen the surface.
	for x = chunk.x-1,chunk.x+w do
		for z = chunk.z-1,chunk.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
	-- Generate civilization obstacles.
	if civ_y then
		-- Calculate the position.
		p:set_xyz(chunk.x + civ_x + 0.5, 0.0, chunk.z + civ_z + 0.5)
		p:multiply(chunk.manager.grid_size)
		p:add_xyz(0, civ_y, 0)
		-- Choose and create the obstacle.
		MapUtils:place_obstacle{point = p, category = "civilization"}
	end
end

--- Gets the dungeon type of the given chunk.
-- @param self TerrainGenerator.
-- @param manager TerrainManager.
-- @param x Chunk offset in grid units.
-- @param z Chunk offset in grid units.
-- @return Chunk type, Y offset and height. Nil if no dungeon exists.
TerrainGenerator.get_dungeon_type = function(self, manager, x, z)
	local cx = math.floor(x / manager.chunk_size)
	local cz = math.floor(z / manager.chunk_size)
	if cx % 2 == 0 and cz % 2 == 0 then
		-- Choose the dungeon amount of the region.
		local r = Noise:plasma_noise_2d(self.seed1 + 0.01 * x, self.seed1 + 0.01 * z, 3)
		if r < 0 then return end
		-- Choose the room height.
		local h = math.abs(Noise:plasma_noise_2d(self.seed1 + 0.035 * x, self.seed1 + 0.035 * z, 2))
		if h < 0.15 then return end
		h = h * 8 + 8
		-- Choose the roughness of the dungeon.
		local r = Noise:plasma_noise_2d(self.seed1 + 0.01 * x, self.seed1 + 0.01 * z, 2)
		local p = 0.5 + 0.35 * r
		-- Choose the vertical offset.
		local y1 = math.abs(Noise:harmonic_noise_2d(self.seed1 + 0.002 * x, self.seed1 + 0.002 * z, 6, 1.3, p))
		local y2 = math.abs(Noise:harmonic_noise_2d(self.seed1 + 0.0002 * x, self.seed1 + 0.0002 * z, 6, 1.3, 0.5))
		local y = 120 + 70 * y1 - 140 * y2
		-- TODO: Choose the dungeon type.
		local t = 1
		return t,y,y,y,y,h,h,h,h
	elseif cx % 2 == 0 and cz % 2 == 1 then
		-- Vertical connection dungeon.
		local at,ay1,ay2,ay3,ay4,ah1,ah2,ah3,ah4 = self:get_dungeon_type(manager, x, z + manager.chunk_size)
		if not at then return end
		local bt,by1,by2,by3,by4,bh1,bh2,bh3,bh4 = self:get_dungeon_type(manager, x, z - manager.chunk_size)
		if not bt then return end
		-- TODO: Choose the dungeon type.
		local t = 2
		return t,ay3,ay4,by1,by2,ah3,ah4,bh1,bh2
	elseif cx % 2 == 1 and cz % 2 == 0 then
		-- Horizontal connection dungeon.
		local at,ay1,ay2,ay3,ay4,ah1,ah2,ah3,ah4 = self:get_dungeon_type(manager, x + manager.chunk_size, z)
		if not at then return end
		local bt,by1,by2,by3,by4,bh1,bh2,bh3,bh4 = self:get_dungeon_type(manager, x - manager.chunk_size, z)
		if not bt then return end
		-- TODO: Choose the dungeon type.
		local t = 3
		return t,by2,ay1,by4,ay3,bh2,ah1,bh4,ah3
	end
end

--- Loads the terrain generator state.
-- @param self TerrainGenerator.
-- @param db Database.
TerrainGenerator.load = function(self, db)
	local r1 = db:query("SELECT key,value FROM generator_settings;")
	local f1 =
	{
		seed1 = function(v) self.seed1 = tonumber(v) end,
		seed2 = function(v) self.seed2 = tonumber(v) end,
		seed3 = function(v) self.seed3 = tonumber(v) end,
		seed4 = function(v) self.seed4 = tonumber(v) end,
		seed5 = function(v) self.seed5 = tonumber(v) end,
		seed6 = function(v) self.seed6 = tonumber(v) end,
		seed7 = function(v) self.seed7 = tonumber(v) end,
		seed8 = function(v) self.seed8 = tonumber(v) end,
		seed9 = function(v) self.seed9 = tonumber(v) end,
		seed10 = function(v) self.seed10 = tonumber(v) end
	}
	for k,v in ipairs(r1) do
		local f = f1[v[1]]
		if f then f(v[2]) end
	end
end

--- Saves the terrain generator state.
-- @param self TerrainGenerator.
-- @param db Database.
-- @param erase True to erase existing data. False otherwise.
TerrainGenerator.save = function(self, db, erase)
	db:query("BEGIN TRANSACTION;")
	if erase then
		db:query([[DROP TABLE IF EXISTS generator_settings;]])
		db:query([[CREATE TABLE generator_settings (
			key TEXT PRIMARY KEY,
			value TEXT);]])
	end
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed1", tostring(self.seed1)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed2", tostring(self.seed2)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed3", tostring(self.seed3)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed4", tostring(self.seed4)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed5", tostring(self.seed5)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed6", tostring(self.seed6)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed7", tostring(self.seed7)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed8", tostring(self.seed8)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed9", tostring(self.seed9)})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed10", tostring(self.seed10)})
	db:query("END TRANSACTION;")
end

return TerrainGenerator
