require "common/chatcommand"

-- God mode.
ChatCommand{pattern = "^/god$", permission = "admin", func = function(player, matches)
	if player.god then
		player:send("/god mode off.")
		player.god = nil
	else
		player:send("/god mode on.")
		player.god = true
	end
end}

-- Grant admin privileges.
ChatCommand{pattern = "^/grant admin ([a-zA-Z0-9]*)$", permission = "admin", func = function(player, matches)
	if not Config.inst.admins[matches] then
		Config.inst.admins[matches] = true
		Config.inst:save()
		player:send("Admin privileges have been granted to " .. matches)
		local affected = Account.dict_name[matches]
		if affected and affected.client then
			Network:send{client = affected.client, packet = Packet(packets.ADMIN_PRIVILEGE, "bool", true)}
		end
	else
		player:send("Admin privileges have already been granted to " .. matches)
	end
end}

-- No clip mode.
ChatCommand{pattern = "^/noclip$", permission = "admin", func = function(player, matches)
	if player.noclip then
		player:send("/noclip mode off.")
		player.noclip = nil
		player.flying = player.spec.flying or false
		player.collision_mask = 0xFFFF
		player.collision_group = 0x0001
		player.gravity = player.spec.gravity
		player.gravity_liquid = player.spec.water_gravity
	else
		player:send("/noclip mode on.")
		player.noclip = true
		player.flying = true
		player.collision_mask = 0
		player.collision_group = 0
		player.gravity = Vector()
		player.gravity_liquid = Vector()
	end
end}

-- Revoke admin privileges.
ChatCommand{pattern = "^/revoke admin ([a-zA-Z0-9]*)$", permission = "admin", func = function(player, matches)
	if Config.inst.admins[matches] then
		Config.inst.admins[matches] = nil
		Config.inst:save()
		player:send("Admin privileges have been revoked from " .. matches)
		local affected = Account.dict_name[matches]
		if affected and affected.client then
			Network:send{client = affected.client, packet = Packet(packets.ADMIN_PRIVILEGE, "bool", false)}
		end
	else
		player:send("Admin privileges have already been revoked from " .. matches)
	end
end}

-- Spawn item.
ChatCommand{pattern = "^/spawn item (.*)$", permission = "admin", func = function(player, matches)
	local spec = Itemspec:find{name = matches[1]}
	if not spec then return end
	Item{
		spec = spec,
		position = player.position,
		random = true,
		realized = true}
end}

-- Spawn obstacles.
ChatCommand{pattern = "^/spawn obstacle (.*)$", permission = "admin", func = function(player, matches)
	local spec = Obstaclespec:find{name = matches[1]}
	if not spec then return end
	Obstacle{
		spec = spec,
		position = player.position,
		realized = true}
end}

-- Spawn species.
ChatCommand{pattern = "^/spawn species (.*)$", permission = "admin", func = function(player, matches)
	local spec = Species:find{name = matches[1]}
	if not spec then return end
	Creature{
		spec = spec,
		position = player.position,
		random = true,
		realized = true}
end}

-- Spawn any object.
ChatCommand{pattern = "^/spawn (.*)$", permission = "admin", func = function(player, matches)
	local spec1 = Species:find{name = matches[1]}
	local spec2 = Itemspec:find{name = matches[1]}
	local spec3 = Obstaclespec:find{name = matches[1]}
	if spec1 then
		Creature{
			spec = spec1,
			position = player.position,
			random = true,
			realized = true}
	elseif spec2 then
		Item{
			spec = spec2,
			position = player.position,
			random = true,
			realized = true}
	elseif spec3 then
		Obstacle{
			spec = spec3,
			position = player.position,
			realized = true}
	end
end}

-- Suicide.
ChatCommand{pattern = "^/suicide$", permission = "player", func = function(player, matches)
	player:die()
end}

ChatCommand{pattern = "^/wireframe$", permission = "player", func = function(player, matches)
	Los.Wireframe_on()
end}

-- Teleportation.
ChatCommand{pattern = "^/teleport (.*)$", permission = "admin", func = function(player, matches)
	if player:teleport{marker = matches[1]} then
		player:send{packet = Packet(packets.MESSAGE, "string", "/teleport: Teleported to " .. matches[1] .. ".")}
	else
		player:send{packet = Packet(packets.MESSAGE, "string", "/teleport: Map marker " .. matches[1] .. " doesn't exist.")}
	end
end}

-- Unlock a random skill, spell type or spell effect.
ChatCommand{pattern = "^/unlock$", permission = "admin", func = function(player, matches)
	Unlocks:unlock_random()
end}

-- Unlock all skills, spell types or spell effects.
ChatCommand{pattern = "^/unlock all$", permission = "admin", func = function(player, matches)
	repeat until not Unlocks:unlock_random()
end}

-- Unlock a map marker.
ChatCommand{pattern = "^/unlock marker (.*)$", permission = "admin", func = function(player, matches)
	local m = Marker:find{name = matches[1]}
	if m then
		m:unlock()
	else
		player:send(string.format("No such map marker %q.", matches[1]))
	end
end}

-- Unlock a skill.
ChatCommand{pattern = "^/unlock skill (.*)$", permission = "admin", func = function(player, matches)
	local spec = Skillspec:find{name = matches[1]}
	if spec then
		Unlocks:unlock("skill", matches[1])
	else
		player:send(string.format("No such skill %q.", matches[1]))
	end
end}

-- Unlock a spell effect.
ChatCommand{pattern = "^/unlock spell effect (.*)$", permission = "admin", func = function(player, matches)
	local spec = Feateffectspec:find{name = matches[1]}
	if spec then
		Unlocks:unlock("spell effect", matches[1])
	else
		player:send(string.format("No such spell effect %q.", matches[1]))
	end
end}

-- Unlock a spell type.
ChatCommand{pattern = "^/unlock spell type (.*)$", permission = "admin", func = function(player, matches)
	local spec = Featanimspec:find{name = matches[1]}
	if spec then
		Unlocks:unlock("spell type", matches[1])
	else
		player:send(string.format("No such spell type %q.", matches[1]))
	end
end}

-- Any other command.
ChatCommand{pattern = "^(/[^ ]*).*", permission = "player", func = function(player, matches)
	player:send("Unrecognized command.")
end}

-- Normal chat.
ChatCommand{pattern = ".*", permission = "player", func = function(player, matches)
	player:say(matches[1])
end}

Protocol:add_handler{type = "PLAYER_CHAT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	local ok,msg = args.packet:read("string")
	if ok then
		local cmd,match = ChatCommand:find(msg)
		if cmd then
			if cmd.permission == "admin" and not player.admin then
				return player:send("You have no permission to do that.")
			end
			return cmd.func(player, {match})
		end
	end
end}
