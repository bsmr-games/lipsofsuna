Message{
	name = "server stats",
	client_to_server_encode = function(self)
		return {}
	end,
	client_to_server_decode = function(self, packet)
		return {}
	end,
	client_to_server_handle = function(self, client)
		-- Check for permissions.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if not player.admin then
			Game.messaging:server_event("message", "You have no permission to do that.")
			return
		end
		-- Count objects.
		local num_players_miss = 0
		local num_players_real = 0
		local num_actors_idle = 0
		local num_actors_miss = 0
		local num_actors_real = 0
		local num_items_miss = 0
		local num_items_inv = 0
		local num_items_real = 0
		local num_obstacles_miss = 0
		local num_obstacles_real = 0
		local num_objects_miss = 0
		local num_objects_real = 0
		local num_vision_miss = 0
		local num_vision_real = 0
		for k,v in pairs(SimulationObject.objects) do
			if v.class_name == "Player" then
				if v:get_visible() then
					num_players_real = num_players_real + 1
				else
					num_players_miss = num_players_miss + 1
				end
				if v.vision then
					for k1,v1 in pairs(v.vision.objects) do
						if k1:get_visible() then
							num_vision_real = num_vision_real + 1
						else
							num_vision_miss = num_vision_miss + 1
						end
					end
				end
			elseif v.class_name == "Actor" then
				if v:get_visible() then
					if v.ai and v.ai.state ~= "none" then
						num_actors_real = num_actors_real + 1
					else
						num_actors_idle = num_actors_idle + 1
					end
				else
					num_actors_miss = num_actors_miss + 1
				end
			elseif v.class_name == "Item" then
				if v:get_visible() then
					num_items_real = num_items_real + 1
				elseif v.parent then
					num_items_inv = num_items_inv + 1
				else
					num_items_miss = num_items_miss + 1
				end
			elseif v.class_name == "Obstacle" then
				if v:get_visible() then
					num_obstacles_real = num_obstacles_real + 1
				else
					num_obstacles_miss = num_obstacles_miss + 1
				end
			else
				if v:get_visible() then
					num_objects_real = num_objects_real + 1
				else
					num_objects_miss = num_objects_miss + 1
				end
			end
		end
		-- Count sectors.
		local num_sectors = 0
		for k,v in pairs(Game.sectors.sectors) do
			num_sectors = num_sectors + 1
		end
		-- Send stats.
		local response = string.format([[FPS: %.2f
Database memory: %d kB
Script memory: %d kB
Terrain memory: %d kB
Update tick: %d ms
Event tick: %d ms
Players: %d+%d
Actors: %d+%d+%d
Items: %d+%d+%d
Obstacles: %d+%d
Others: %d+%d
Vision: %d+%d
Sectors: %d]],
			Program.fps, Database.memory_used / 1024, collectgarbage("count") / 1024, Voxel.memory_used / 1024,
			1000 * Program.profiling.update, 1000 * Program.profiling.event,
			num_players_real, num_players_miss,
			num_actors_real, num_actors_idle, num_actors_miss,
			num_items_real, num_items_inv, num_items_miss,
			num_obstacles_real, num_obstacles_miss,
			num_objects_real, num_objects_miss,
			num_vision_real, num_vision_miss,
			num_sectors,
			Program.profiling.update * 1000, Program.profiling.event * 1000)
		Game.messaging:server_event("server stats", response)
	end,
	server_to_client = function(self, text)
		return {"string", text}
	end,
	server_to_client = function(self, packet)
		local ok,text = packet:read("string")
		if not ok then return end
		return {text}
	end,
	server_to_client = function(self, text)
		Client.data.admin.server_stats = text
		Ui.state = "admin/server-stats"
	end}