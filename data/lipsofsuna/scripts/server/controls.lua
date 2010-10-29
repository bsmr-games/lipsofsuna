Protocol:add_handler{type = "CHAT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	local ok,msg = args.packet:read("string")
	if ok then
		player:say(msg)
	end
end}

Protocol:add_handler{type = "CHARACTER_CREATE", func = function(args)
	-- Make sure not created already.
	local player = Player:find{client = args.client}
	if player then return end
	-- Get character flags.
	local ok,ra,ge,hs,hc = args.packet:read("string", "string", "string", "string")
	if not ok then return end
	local species = Species:find{name = ra .. "-player"}
	if not species then return end
	-- TODO: Input validation.
	Network:send{client = args.client, packet = Packet(packets.CHARACTER_ACCEPT)}
	-- Create character.
	local o = Player{
		client = args.client,
		gender = ge,
		hair_color = hc,
		hair_style = hs,
		position = Config.spawn,
		species = species,
		realized = true}
	Player.clients[args.client] = o
	-- Transmit active and completed quests.
	for k,q in pairs(Quest.dict_name) do
		if q.status == "active" then
			o:send{packet = Packet(packets.QUEST_ACTIVATE, "uint32", q.id)}
		elseif q.status == "completed" then
			o:send{packet = Packet(packets.QUEST_COMPLETE, "uint32", q.id)}
		end
	end
end}

Protocol:add_handler{type = "CROUCH", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,on = args.packet:read("bool")
		if ok and on then
			player:animate{animation = "crouch", channel = Animation.CHANNEL_CROUCH, weight = 0.5, permanent = true}
			player.crouch = true
		else
			player:animate{channel = Animation.CHANNEL_CROUCH}
			player.crouch = nil
		end
	end
end}

Protocol:add_handler{type = "EXAMINE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,inv,slot = args.packet:read("uint32", "uint32")
		if ok then
			local object = player:find_target(inv, slot)
			if object then
				if object.examine_cb then
					object:examine_cb(player)
				else
					player:send{packet = Packet(packets.MESSAGE, "string",
						"There's nothing special about this object.")}
				end
			end
		end
	end
end}

Protocol:add_handler{type = "JUMP", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		player:jump()
	end
end}

Protocol:add_handler{type = "MOVE_ITEM", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local cmd = {}
		local ok,src,dst = args.packet:read("uint8", "uint8")
		if not ok then return end
		if src == moveitem.EQUIPMENT then
			ok,cmd.srcslot = args.packet:resume("string")
			if not ok then return end
		elseif src == moveitem.INVENTORY then
			ok,cmd.srcinv,cmd.srcslot = args.packet:resume("uint32", "uint32")
			if not ok then return end
		elseif src == moveitem.WORLD then
			ok,cmd.srcobj = args.packet:resume("uint32")
			if not ok then return end
		else return end
		if dst == moveitem.EQUIPMENT then
			ok,cmd.dstslot = args.packet:resume("string")
			if not ok then return end
		elseif dst == moveitem.INVENTORY then
			ok,cmd.dstinv,cmd.dstslot = args.packet:resume("uint32", "uint32")
			if not ok then return end
			if cmd.dstslot == 0 then cmd.dstslot = nil end
		elseif dst == moveitem.WORLD then
			local ok1,x,y,z,v = args.packet:resume("float", "float", "float", "float")
			if not ok1 then return end
			if v < -10 then v = -10 end
			if v > 10 then v = 10 end
			cmd.dstpoint = Vector(x, y, z)
			cmd.rotation = player.rotation
			cmd.velocity = player.rotation * Vector(0, 0, v)
		else return end
		cmd.object = player
		Actions:moveitem(cmd)
	end
end}

Protocol:add_handler{type = "PLAYER_MOVE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,v = args.packet:read("int8")
		if ok then
			if v > 0 then
				player:set_movement(1)
			elseif v < 0 then
				player:set_movement(-1)
			else
				player:set_movement(0)
			end
		end
	end
end}

Protocol:add_handler{type = "PLAYER_TURN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,x,y,z,w = args.packet:read("float", "float", "float", "float")
		if ok then
			local e = Quaternion(x, y, z, w).euler
			e[3] = math.min(player.species.tilt_limit, e[3])
			e[3] = math.max(-player.species.tilt_limit, e[3])
			player.rotation = Quaternion:new_euler(e)
		end
	end
end}

Protocol:add_handler{type = "RUN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,on = args.packet:read("bool")
		if ok then
			player.running = on
			player:calculate_speed()
		end
	end
end}

Protocol:add_handler{type = "SKILLS", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		-- Read packet data.
		local ok,s,v = args.packet:read("string", "float")
		if not ok then return end
		if v < 0 then return end
		-- Enforce species limit.
		local spec = player.species.skills[s]
		if not spec then return end
		v = math.min(v, spec.max)
		-- Enforce skill quota.
		local skills = player.skills
		if not skills then return end
		local t = skills:get_total() - skills:get_maximum{skill = s}
		v = math.min(v, player.species.skill_quota - t)
		if v <= 0 then return end
		-- Set the new maximum value.
		skills:set_maximum{skill = s, value = v}
	end
end}

Protocol:add_handler{type = "SHOOT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		Feat:perform{name = "attack", user = player}
	else
		player:respawn()
	end
end}

Protocol:add_handler{type = "SKILL", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,id,on = args.packet:read("string", "bool")
		if ok then
			Feat:perform{name = id, stop = not on, user = player}
		end
	end
end}

Protocol:add_handler{type = "STRAFE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,val = args.packet:read("int8")
		if ok then
			player:set_strafing(val / 127)
		end
	end
end}

Protocol:add_handler{type = "THROW", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,inv,slot = args.packet:read("uint32", "uint32")
		if ok then player:throw(inv, slot) end
	end
end}

Protocol:add_handler{type = "USE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,inv,slot = args.packet:read("uint32", "uint32")
		if ok then
			local object = player:find_target(inv, slot)
			if object and object.use_cb then
				object:use_cb(player)
			end
		end
	end
end}
