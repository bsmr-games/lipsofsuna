--- Joins multiplayer games.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.multiplayer.join
-- @alias Join

local Class = require("system/class")
local Client = require("core/client/client")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Network = require("system/network")
local Ui = require("ui/ui")

--- Joins multiplayer games.
-- @type Join
local Join = Class("Join")

--- Creates a new multiplayer joiner.
-- @param clss Join class.
-- @return Join.
Join.new = function(clss)
	local self = Class.new(clss)
	self.data = {}
	self.data.address = "localhost"
	self.data.port = 10101
	return self
end

--- Called when logging in has failed.
-- @param self Join.
-- @param message Error message.
Join.login_failed = function(self, message)
	self.data.status = "Login failed: " .. message
	self.data.state = "failed"
	Ui:restart_state()
end

--- Switches the game to the joining start state.
-- @param self Join.
-- @param addr Address.
-- @param port Port.
Join.join_game = function(self, addr, port)
	-- Initialize the state.
	self.data.address = addr or self.data.address
	self.data.port = port or self.data.port
	self.data.state = "init"
	self.data.remote = self.data.address .. ":" .. self.data.port
	self.data.status = "Joining the server at " .. self.data.remote .. "..."
	-- Initialize the game.
	Main.game = Game("join", nil, self.data.port)
	Main.game:start()
	Main.messaging:set_transmit_mode(false, true, self.data.port)
	Main.game_start_hooks:call()
	-- Initialize terrain synchronization.
	Main.terrain.unload_time = nil --FIXME
	Main.terrain:set_chunk_generator(function(chunk)
		Client.terrain_sync:load_chunk(chunk.x, chunk.z)
	end)
	-- Enter the connection state.
	Ui:set_state("join/connect")
end

--- Updates the joining status.
-- @param self Join.
-- @return True if the state changed. False otherwise.
Join.update = function(self, secs)
	if self.data.state == "init" then
		-- Initialize the connection.
		if Network:join{host = self.data.address, port = self.data.port} then
			self.data.status = "Connecting to " .. self.data.remote .. "..."
			self.data.state = "connecting"
			Ui:restart_state()
			return true
		else
			self.data.status = "Failed to connect to " .. self.data.remote .. "!"
			self.data.state = "failed"
			Ui:restart_state()
			return true
		end
	elseif self.data.state == "connecting" then
		-- Wait for the connection to establish.
		if not Network:get_connected() then return end
		self.data.status = "Waiting for reply from " .. self.data.remote .. "..."
		self.data.state = "waiting"
		Ui:restart_state()
		return true
	elseif self.data.state == "waiting" then
		-- Wait for the login packet.
		if Network:get_connected() then return end
		self.data.status = "Connection lost to " .. self.data.remote .. "!"
		self.data.state = "failed"
		return true
	end
end

--- Gets the status message.
-- @param self Join.
-- @return String.
Join.get_status = function(self)
	return self.data.status
end

return Join
