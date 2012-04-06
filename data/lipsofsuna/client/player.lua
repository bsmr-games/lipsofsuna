Player = Class()
Player.species = "aer" -- FIXME

local radian_wrap = function(x)
	local y = x
	while y < -math.pi do
		y = y + 2 * math.pi
	end
	while y > math.pi do
		y = y - 2 * math.pi
	end
	return y
end

Player.get_ignored_objects = function(clss)
	local ignore = {clss.object}
	local slots = clss.object.slots
	if slots then
		for k,v in pairs(slots.slots) do
			table.insert(ignore, v)
		end
	end
	return ignore
end

Player.pick_look = function(clss)
	-- Make sure that the player is logged in.
	if not clss.object then
		Target.target_object = nil
		return
	end
	-- Ray pick an object in front of the player.
	local r1,r2 = Client.camera1:get_picking_ray()
	if not r1 then return end
	local p,o = Target:pick_ray{ray1 = r1, ray2 = r2}
	-- Update highlighting.
	if o ~= Target.target_object and Object.set_effect then
		if Target.target_object then
			Target.target_object:set_effect()
		end
		if o then
			o:set_effect{shader = "highlight"}
		end
	end
	Target.target_object = o
	-- Update the interaction text.
	if o and o.spec and o.spec.interactive then
		local action = Binding.dict_name["use"]
		local key = action and action.key1 and Keycode[action.key1] or "--"
		if o.name and o.name ~= "" then
			Client:set_target_text(string.format("[%s] Interact with %s", key, o.name))
		elseif o.spec.name then
			Client:set_target_text(string.format("[%s] Interact with %s", key, o.spec.name))
		else
			Client:set_target_text(string.format("[%s] Interact", key))
		end
		set = true
	else
		Client:set_target_text()
	end
	-- Update the 3D crosshair position.
	clss.crosshair_position = (p or r2) - (r2 - r1):normalize() * 0.1
end

Player.tilt = 0
Player.turn = 0
Player.tilt_state = 0
Player.turn_state = 0
Player.rotation_curr = Quaternion()
Player.rotation_prev = Quaternion()
Player.rotation_sync_timer = 0
Player.update_rotation = function(clss, secs)
	if clss.object.dead then return end
	local spec = Player.object.spec
	-- Update turning.
	clss.turn_state = clss.turn_state + clss.turn * secs
	clss.turn_state = radian_wrap(clss.turn_state)
	clss.turn = 0
	-- Update tilting.
	clss.tilt_state = clss.tilt_state + clss.tilt * secs
	if spec then
		clss.tilt_state = math.min(spec.tilt_limit, clss.tilt_state)
		clss.tilt_state = math.max(-spec.tilt_limit, clss.tilt_state)
	end
	clss.tilt = 0
	-- Update rotation.
	local rot = Quaternion{euler = {clss.turn_state, 0, 0}}
	clss.object:update_rotation(rot, -clss.tilt_state)
	-- Sync rotation with the server.
	-- Rotation takes at most 0.25 seconds to fully synchronize. Large changes
	-- are sent immediately whereas smaller changes are grouped together to
	-- reduce useless network traffic.
	clss.rotation_curr = Quaternion{euler = {clss.turn_state, 0, -clss.tilt_state}}
	clss.rotation_sync_timer = clss.rotation_sync_timer + secs
	if (clss.rotation_prev - clss.rotation_curr).length > math.max(0, 0.1 - 0.4 * clss.rotation_sync_timer) then
		clss:send_rotation()
	end
end

Player.send_rotation = function(clss)
	local r = clss.rotation_curr
	clss.rotation_prev = r
	Network:send{packet = Packet(packets.PLAYER_TURN, "float", r.x, "float", r.y, "float", r.z, "float", r.w)}
end

Player.update_pose = function(clss, secs)
end
