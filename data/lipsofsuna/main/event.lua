local Eventhandler = require("system/eventhandler")
local Lobby = require("system/lobby")
local Network = require("system/network")
local Object = require("system/object")
local Timer = require("system/timer")
local Vision = require("system/vision")

local handle_vision = function(self, args)
	Vision:dispatch_event(args, Main.objects)
end

local handle_motion = function(self, args)
	-- Find the object.
	local object = Main.objects:find_by_id(args.id)
	if not object then return end
	-- Synchronize the transformation.
	Object.set_position(object, object.physics:get_position())
	Object.set_rotation(object, object.physics:get_rotation())
	-- Emit a vision event.
	args.object = object
	handle_vision(self, args)
end

Eventhandler{type = "object-motion", func = handle_motion}
Eventhandler{type = "vision-object-shown", func = handle_vision}
Eventhandler{type = "vision-object-hidden", func = handle_vision}

Eventhandler{type = "quit", func = function(self, args)
	Program:set_quit(true)
end}

Eventhandler{type = "login", func = function(self, event)
	Main.log:format("Client connect from %q.", Network:get_client_address(event.client) or "???")
	-- Tell the client to authenticate.
	Main.messaging:server_event("login", event.client)
	-- Update lobby.
	Lobby:set_players(Lobby:get_players() + 1)
end}

Eventhandler{type = "logout", func = function(self, event)
	Main.log:text("Client disconnect.")
	Server:remove_client(event.client)
end}

Eventhandler{type = "object-contact", func = function(self, event)
	if event.self then
		event.self = Main.objects:find_by_id(event.self)
		if not event.self then return end
	end
	if event.object then
		event.object = Main.objects:find_by_id(event.object)
		if not event.object then return end
	end
	if event.self.contact_cb then
		event.self:contact_cb(event)
	end
end}

Eventhandler{type = "packet", func = function(self, args)
	Main.messaging:handle_packet(args.client, args.packet)
end}

Eventhandler{type = "sector-load", func = function(self, args)
	if Main.game then
		Main.objects:load_chunk(args.sector)
	end
end}

Timer{delay = 3000, func = function()
	if Server.initialized then
		Lobby:upload_server_info()
	end
end}
