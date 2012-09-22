--- Implements asynchronous terrain chunk loading.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.terrain_chunk_loader
-- @alias TerrainChunkLoader

local Class = require("system/class")

--- Implements asynchronous terrain chunk loading.
-- @type TerrainChunkLoader
local TerrainChunkLoader = Class("TerrainChunkLoader")

--- Creates a new terrain chunk loader.
-- @param clss TerrainChunkLoader class.
-- @param manager TerrainManager.
-- @return TerrainChunkLoader.
TerrainChunkLoader.new = function(clss, manager, id, x, z)
	local self = Class.new(clss)
	self.manager = manager
	self.id = id
	self.x = x
	self.z = z
	self.routine = coroutine.create(function()
		self:execute(coroutine.yield)
	end)
	return self
end

--- Executes the loading process.<br/>
--
-- This can be called either synchronously or asynchronously by providing
-- the desired yield function. When run in a coroutine, coroutine.yield()
-- should be used. When running synchronously, a dummy function should be
-- used.
--
-- @param clss TerrainChunkLoader class.
-- @param yield Function.
TerrainChunkLoader.execute = function(self, yield)
	-- Read the chunk from the database.
	if self.manager.database then
		local rows = self.manager.database:query([[
			SELECT * FROM terrain_chunks
			WHERE id=?;]], {self.id})
		yield()
		if rows and #rows ~= 0 then
			self.manager.terrain:load_chunk(self.x, self.z)
			for k,v in ipairs(rows) do
				if self.manager.terrain:set_chunk_data(self.x, self.z, v[2]) then
					return
				end
			end
		end
	end
	-- Create new chunk contents.
	if self.manager.generate then
		-- Generate a random chunk.
		self.manager.terrain:load_chunk(self.x, self.z)
		self:generate_random()
		yield()
	else
		-- Create an empty chunk.
		self.manager.terrain:load_chunk(self.x, self.z)
	end
end

--- Forces the loader to run until finished.
-- @param self TerrainChunkLoader.
TerrainChunkLoader.finish = function(self)
	repeat until not self:update(1)
	self.routine = nil
end

--- Generates random terrain for the chunk.
-- @param self TerrainChunkLoader.
TerrainChunkLoader.generate_random = function(self)
	-- FIXME: Call the generator instead.
	local Noise = require("system/noise")
	local bh = 1695
	local get_height_at = function(x, z)
		local h1 = Noise:perlin_noise(Vector(x,z), Vector(0.05,0.05), 1, 3, 0.5, 2345235)
		local h2 = Noise:perlin_noise(Vector(555-x,344-z), Vector(0.1,0.1), 1, 3, 0.5, 43566)
		return bh + 9 + h1, math.max(0, 5 * h2)
	end
	local w = self.manager.chunk_size
	local t = self.manager.terrain
	for x = self.x,self.x+w-1 do
		for z = self.z,self.z+w-1 do
			local a00,b00 = get_height_at(x, z)
			local a10,b10 = get_height_at(x + 1, z)
			local a01,b01 = get_height_at(x, z + 1)
			local a11,b11 = get_height_at(x + 1, z + 1)
			if b00 > 0 or b10 > 0 or b01 > 0 or b11 > 0 then
				local m = Noise:perlin_noise(Vector(234-x,435-z), Vector(0.05,0.05), 1, 3, 0.5, 1234)
				m = math.min(math.max(1, 4 * math.abs(m)), 3)
				t:add_stick_corners(x, z, 0, 0, 0, 0, a00 + b00, a10 + b10, a01 + b01, a11 + b11, m)
			end
			t:add_stick_corners(x, z, 0, 0, 0, 0, a00, a10, a01, a11, 3)
		end
	end
	for x = self.x-1,self.x+w do
		for z = self.z-1,self.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
end

--- Updates the sector loader.
-- @param self TerrainChunkLoader.
-- @param secs Seconds since the last update.
-- @return True if still loading, false if finished.
TerrainChunkLoader.update = function(self, secs)
	if not self.routine then return end
	local ret,err = coroutine.resume(self.routine, secs)
	if not ret then print(debug.traceback(self.routine, err)) end
	if coroutine.status(self.routine) == "dead" then
		self.routine = nil
		return
	end
	return true
end

return TerrainChunkLoader