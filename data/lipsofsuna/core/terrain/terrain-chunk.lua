--- Stores information on a terrain chunk.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.terrain_chunk
-- @alias TerrainChunk

local Class = require("system/class")
local Packet = require("system/packet")
local Program = require("system/core")
local TerrainChunkLoader = require("core/terrain/terrain-chunk-loader")
local Time = require("system/time")
local Vector = require("system/math/vector")

--- Stores information on a terrain chunk.
-- @type TerrainChunk
local TerrainChunk = Class("TerrainChunk")

--- Creates a new terrain chunk.
-- @param clss TerrainChunk class.
-- @param manager TerrainManager.
-- @return TerrainChunk.
TerrainChunk.new = function(clss, manager, x, z)
	local self = Class.new(clss)
	self.manager = manager
	self.x = x
	self.z = z
	self.id = manager:get_chunk_id_by_xz(x, z)
	self.time = Time:get_secs()
	self.loader = TerrainChunkLoader(manager, self.id, x, z)
	return self
end

--- Creates the render object for the chunk.
-- @param self TerrainChunk.
TerrainChunk.create_render_object = function(self)
	-- Create the object.
	local size = self.manager.grid_size
	if not self.object then
		local RenderObject = require("system/render-object")
		self.object = RenderObject()
	end
	-- Create the model.
	local prev_model = self.model
	self.model = self.manager.terrain:build_chunk_model(self.x, self.z)
	-- Assign the model.
	if prev_model then
		self.__prev_model = prev_model
		self.object:replace_model(prev_model:get_render(), self.model:get_render())
	else
		self.object:add_model(self.model:get_render())
	end
	self.object:set_visible(true)
end

--- Detaches the chunk from the scene.
-- @param self TerrainChunk.
TerrainChunk.detach = function(self)
	-- Chain detach the object chunk.
	if Main and Main.objects then
		local p = self.manager:get_chunk_center_by_xz(self.x, self.z)
		local x,z = Main.objects.chunks:get_chunk_xz_by_point(p.x, p.z)
		Main.objects.chunks:unload_chunk(x, z)
	end
	-- Detach the render object.
	if self.manager.graphics then
		self:detach_render_object()
	end
	-- Unload the terrain chunk.
	self.manager.terrain:unload_chunk(self.x, self.z)
end

--- Detaches the render object from the scene.
-- @param self TerrainChunk.
TerrainChunk.detach_render_object = function(self)
	-- Free the object and the model.
	if self.object then
		self.object:set_visible(false)
		self.object = nil
	end
	self.manager.terrain:clear_chunk_model(self.x, self.z)
	self.model = nil
	self.__prev_model = nil
	-- Clear the timestamp.
	self.time_model = nil
end

--- Progresses the loading of the chunk.
-- @param self TerrainChunk.
TerrainChunk.load = function(self)
	if not self.loader then return end
	if not self.loader:update(secs) then
		self.loader = nil
	end
end

--- Saves the chunk.
-- @param self TerrainChunk.
TerrainChunk.save = function(self)
	-- Skip chunks that are still loading.
	if not self.manager.database then return end
	if self.loader then return end
	-- Write the chunk data.
	local data = Packet(1)
	if self.manager.terrain:get_chunk_data(self.x, self.z, data) then
		self.manager.database:query([[
			REPLACE INTO terrain_chunks (id,data)
			VALUES (?,?);]], {self.id, data})
	else
		print("ERROR: Could not save terrain")
	end
	-- Chain save the object chunk.
	if Main and Main.objects then
		local p = self.manager:get_chunk_center_by_xz(self.x, self.z)
		local x,z = Main.objects.chunks:get_chunk_xz_by_point(p.x, p.z)
		Main.objects.chunks:save_chunk(x, z)
	end
end

--- Updates the chunk.
-- @param self TerrainChunk.
-- @param secs Seconds since the last update.
TerrainChunk.update = function(self, secs)
	local t = Time:get_secs()
	-- Unload unused chunks.
	if self.manager.unload_time and t - self.time > self.manager.unload_time then
		self:save()
		self:detach()
		self.manager.chunks[self.id] = nil
		return
	end
	-- Remove replaced models.
	if self.__prev_model then
		local loaded
		if self.model then
			local r = self.model:get_render()
			loaded = not r or r:get_loaded()
		else
			loaded = true
		end
		if loaded then
			self.__prev_model = nil
		end
	end
	-- Unload unused models.
	if self.time_model and t - self.time_model >= self.manager.unload_time_model then
		self:detach_render_object()
		return
	end
end

return TerrainChunk
