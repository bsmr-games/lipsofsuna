Message{
	name = "use in world",
	client_to_server_encode = function(self, id, action)
		return {"uint32", id, "string", action}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,action = packet:read("uint32", "string")
		if not ok then return end
		return {id, action}
	end,
	client_to_server_handle = function(self, client, id, action)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Find the object.
		local object = Game.objects:find_by_id(id)
		if not object then return end
		if not player:can_reach_object(object) then return end
		-- Validate the action.
		if not object.spec.usages[action] then return end
		action = Actionspec:find{name = action}
		if not action then return end
		if not action.func then return end
		-- Perform the action.
		action.func(object, player)
	end}
