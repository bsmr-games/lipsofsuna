-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "unequip",
	client_to_server_encode = function(self, index)
		return {"uint32", index}
	end,
	client_to_server_decode = function(self, packet)
		local ok,index = packet:read("uint32")
		if not ok then return end
		return {index}
	end,
	client_to_server_handle = function(self, client, index)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the item and validate the equip.
		local object = player.inventory:get_object_by_index(index)
		if not object then return end
		-- Equip the item in the slot.
		player.inventory:unequip_index(index)
	end}
