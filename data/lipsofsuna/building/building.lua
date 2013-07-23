--- Building subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module building.building
-- @alias Building

local Class = require("system/class")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Physics = require("system/physics")
local PhysicsConsts = require("core/server/physics-consts")
local Player = require("core/objects/player")
local ServerUtils = require("core/server/util") --FIXME
local TerrainManager = require("core/terrain/terrain-manager")
local Vector = require("system/math/vector")

--- Building subgame.
-- @type Building
Building = Class("Building")

--- Creates the building subgame.
-- @param clss Building class.
-- @return Building.
Building.new = function(clss)
	local self = Class.new(clss)
	-- Initialize the game.
	Main.messaging:set_transmit_mode(true, true)
	Main.game = Game("benchmark")
	Main.game.sectors.unload_time = nil
	ServerUtils:set_player_spawn_point(Vector(500,101,500))
	-- Initialize the terrain.
	self.terrain = TerrainManager(12, 1, nil, false, true, true)
	self.terrain:set_view_center(Vector(500, 0, 500))
	self.terrain.generate_hooks:register(0, function(self)
		local w = self.manager.chunk_size
		local t = self.manager.terrain
		for z = 0,w-1 do
			for x = 0,w-1 do
				t:add_stick_corners(self.x + x, self.z + z, 0, 0, 0, 0, 100, 100, 100, 100, 3)
			end
		end
		for x = self.x-1,self.x+w do
			for z = self.z-1,self.z+w do
				t:calculate_smooth_normals(x, z)
			end
		end
		return Hooks.STOP
	end)
	Main.terrain = self.terrain --FIXME
	-- Enable the simulation.
	Physics:set_enable_simulation(true)
	return self
end

--- Closes the subgame.
-- @param self Building.
Building.close = function(self)
	Main.terrain:unload_all();
	Main.terrain = nil
	ServerUtils:set_player_spawn_point()
end

--- Updates the subgame state.
-- @param self Building.
-- @param secs Seconds since the last update.
Building.update = function(self, secs)
	-- Initialize the player.
	if not self.player then
		self.player = Player(Main.objects)
		self.player:set_spec(Actorspec:find_by_name("building player"))
		self.player:randomize()
		self.player.get_admin = function() return true end --FIXME
		self.player:set_position(Vector(500,101,500))
		self.player.physics:set_collision_group(PhysicsConsts.GROUP_PLAYERS)
		self.player:set_visible(true)
		self.player:set_client(-1)
		Client:set_player_object(self.player)
		Server.players_by_client = {}
		Server.players_by_client[-1] = self.player --FIXME
		self.player:calculate_animation()
	end
	-- Update lighting.
	Client.lighting:update(secs)
	-- Update terrain.
	self.terrain:refresh_chunks_by_point(Vector(500, 0, 500), 20)
end

return Building
