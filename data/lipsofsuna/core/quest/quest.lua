--- Stores the status of an individual quest.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.quest.quest
-- @alias Quest

local Class = require("system/class")

--- Stores the status of an individual quest
-- @type Quest
local Quest = Class("Quest")

--- Creates a new quest.
-- @param clss Quest class.
-- @param manager Quest manager.
-- @param name Quest name.
-- @param status Status string. Nil for inactive.
-- @param text Description text. Nil for none.
-- @param marker Marker name. Nil for none.
-- @return Quest.
Quest.new = function(clss, manager, name, status, text, marker)
	local self = Class.new(clss)
	self.manager = manager
	self.name = name
	self.status = status or "inactive"
	self.text = text
	self.marker = marker
	return self
end

--- Sends the quest status to the given client.
-- @param self Quest.
-- @param client Client.
-- @param status True to send the status.
-- @param marker True to send the marker.
Quest.send_to_client = function(self, client, status, marker)
	if self.status ~= "active" and self.status ~= "completed" then return end
	if status then
		Main.messaging:server_event("update quest status", client, self.name, self.status, self.text)
	end
	if marker then
		Main.messaging:server_event("update quest marker", client, self.name, self.marker)
	end
end

--- Updates the status, progress, and text of the quest.
-- @param self Quest.
-- @param args Arguments.<ul>
--   <li>progress: Progress number or nil.</li>
--   <li>marker: Map marker name or nil.</li>
--   <li>status: Quest status or nil.</li>
--   <li>text: Quest text or nil.</li></ul>
Quest.update = function(self, args)
	-- Update the status.
	local ch_m = false
	local ch_p = false
	local ch_s = false
	local ch_t = false
	if args.marker and self.marker ~= args.marker then
		local m = Main.markers:find_by_name(args.marker)
		if m then
			if not m.unlocked then
				m:unlock()
			end
			self.marker = args.marker
			ch_m = true
		end
	end
	if args.progress and self.progres ~= args.progress then
		self.progress = args.progress
		ch_p = true
	end
	if args.status and self.status ~= args.status then
		self.status = args.status
		ch_s = true
	end
	if args.text and self.text ~= args.text then
		self.text = args.text
		ch_t = true
	end
	-- Inform all clients.
	for client in pairs(Server.players_by_client) do
		self:send_to_client(client, ch_s or ch_t, ch_m)
	end
	-- Save changes.
	if ch_m or ch_p or ch_s or ch_t then
		self.manager:save_quest(self)
	end
end

return Quest
