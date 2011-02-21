Client.title = "Lips of Suna"
Reload.enabled = true

Eventhandler{type = "select", func = function(self, args)
	args.object.selected = not args.object.selected
end}

Eventhandler{type = "object-model", func = function(self, args)
	--args.object:update_model{model = args.model}
end}

Protocol:add_handler{type = "FEAT_UNLOCK", func = function(event)
	local ok,n = event.packet:read("string")
	if ok then
		local feat = Feat:find{name = n}
		if not feat then return end
		Gui.chat_history:append{text = "Unlocked feat " .. feat.name}
		feat.locked = false
	end
end}

Protocol:add_handler{type = "MESSAGE", func = function(event)
	local ok,msg = event.packet:read("string")
	if ok then
		Gui.chat_history:append{text = msg}
	end
end}

Protocol:add_handler{type = "PARTICLE_RAY", func = function(event)
	local ok,sx,sy,sz,dx,dy,dz,t = event.packet:read("float", "float", "float", "float", "float", "float", "float")
	if ok then
		--Effect:ray{src = Vector(sx, sy, sz), dst = Vector(dx, dy, dz), life = 0.5}
	end
end}

-- Vision management.

Protocol:add_handler{type = "OBJECT_ANIMATED", func = function(event)
	local ok,i,a,c,perm,time,weig = event.packet:read("uint32", "string", "uint8", "bool", "float", "float")
	if ok then
		local o = Object:find{id = i}
		if o then
			local anim = (a ~= "" and a or nil)
			local chan = (c < 255 and c or nil)
			o:animate{animation = anim, channel = chan, fade_in = 0.3, fade_out = 0.3, permanent = perm, time = time, weight = weig}
		end
	end
end}

Protocol:add_handler{type = "OBJECT_DAMAGE", func = function(args)
end}

Protocol:add_handler{type = "OBJECT_EFFECT", func = function(event)
	local ok,i,e = event.packet:read("uint32", "string")
	if ok then
		-- Find the object.
		local obj = Object:find{id = i}
		if not obj then return end
		-- Find the effect.
		local effect = Effect:find{name = e}
		if not effect then return end
		-- Create an effect object.
		EffectObject{
			model = effect.model,
			object = obj,
			sound = effect.sound,
			sound_pitch = effect.sound_pitch,
			realized = true}
	end
end}

Protocol:add_handler{type = "OBJECT_HIDDEN", func = function(event)
	local ok,i = event.packet:read("uint32")
	if ok then
		local o = Object:find{id = i}
		if o then o.realized = false end
	end
end}

Protocol:add_handler{type = "OBJECT_MOVED", func = function(event)
	local ok,i,x,y,z,rx,ry,rz,rw,vx,vy,vz = event.packet:read("uint32", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float")
	if ok then
		local o = Object:find{id = i}
		if not o then return end
		-- Apply position correction.
		-- Objects controlled by physics would normally float in the air
		-- due to collision margins so we need to apply compensation.
		local p = Vector(x, y, z)
		if o.type == "item" or o.type == "species" then p = p + Object.physics_position_correction end
		-- Set the target interpolation position.
		o:set_motion_state(p, Quaternion(rx, ry, rz, rw), Vector(vx, vy, vz))
	end
end}

Protocol:add_handler{type = "OBJECT_SELF", func = function(event)
	local ok,i = event.packet:read("uint32")
	if ok then
		local o = Object:find{id = i}
		if o then
			Player.species = o.race or "aer"
			Camera.mode = "third-person"
			Quickslots:reset()
			Views.Feats.inst:set_race(Player.species)
			Views.Feats.inst:show(1)
		end
		Player.object = o
	end
end}

Protocol:add_handler{type = "OBJECT_SHOWN", func = function(event)
	local ok,i,t,s,m,n,x,y,z,rx,ry,rz,rw = event.packet:read("uint32", "string", "string", "string", "string", "float", "float", "float", "float", "float", "float", "float")
	if not ok then return end
	-- Get the object specification.
	local spec
	if t == "item" then spec = Itemspec:find{name = s}
	elseif t == "obstacle" then spec = Obstaclespec:find{name = s}
	elseif t == "species" then spec = Species:find{name = s} end
	if spec and n == "" then n = spec.name end
	-- Apply position correction.
	-- Objects controlled by physics would normally float in the air
	-- due to collision margins so we need to apply compensation.
	local p = Vector(x, y, z)
	if t == "item" or t == "species" then p = p + Object.physics_position_correction end
	-- Create the object.
	local o = Object{id = i, model = m, name = n, position = p, spec = s, type = t}
	if t == "species" then o.race = s end
	-- Apply optional customizations.
	local ok,ge,bo,no,bu,eye,eyer,eyeg,eyeb,hair,hairr,hairg,hairb,skin,skinr,sking,skinb = event.packet:resume(
		"string", "float", "float", "float",
		"string", "uint8", "uint8", "uint8",
		"string", "uint8", "uint8", "uint8",
		"string", "uint8", "uint8", "uint8")
	if ok then
		o.gender = ge
		o.body_scale = bo
		o.bust_scale = bu
		o.nose_scale = no
		o.eye_style = eye
		o.eye_color = {eyer / 255, eyeg / 255, eyeb / 255}
		o.hair_style = hair
		o.hair_color = {hairr / 255, hairg / 255, hairb / 255}
		o.skin_style = skin
		o.skin_color = {skinr / 255, sking / 255, skinb / 255}
	else
		o.gender = nil
		o.body_scale = nil
		o.bust_scale = nil
		o.nose_scale = nil
		o.eye_style = nil
		o.eye_color = nil
		o.hair_style = nil
		o.hair_color = nil
		o.skin_style = nil
		o.skin_color = nil
	end
	-- Rebuild the model.
	o:update_model()
	o:update_rotation(Quaternion(rx, ry, rz, rw))
	o.realized = true
end}

Protocol:add_handler{type = "OBJECT_SKILL", func = function(event)
	local ok,i,s,v,m = event.packet:read("uint32", "string", "int32", "int32")
	if ok then
		-- Find the affected object.
		local o = Object:find{id = i}
		if not o then return end
		-- Update player skills.
		if o == Player.object then
			Gui.skills:update(s, v, m)
		end
		-- Display health changes.
		if s == "health" then
			if o.health then
				local diff = v - o.health
				if math.abs(diff) > 2 then
					local red = {1,0,0,1}
					local green = {0,1,0,1}
					EffectObject{
						life = 3,
						object = o,
						position = Vector(0,2,0),
						realized = true,
						text = tostring(diff),
						text_color = diff > 0 and green or red,
						text_fade_time = 1,
						text_font = "medium",
						velocity = Vector(0,0.5,0)}
				end
			end
			o.health = v
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SLOT", func = function(event)
	local ok,i,count,spec,slot = event.packet:read("uint32", "uint32", "string", "string")
	if ok then
		local o = Object:find{id = i}
		if not o then return end
		local slots = Slots:find{owner = o} or Slots:new{owner = o}
		spec = Itemspec:find{name = spec}
		-- Update the model.
		if not spec then
			-- Missing spec.
			slots:set_object{slot = slot}
			if o.equipment and o.equipment[slot] then
				o.equipment[slot] = nil
				o:update_model()
			end
		elseif spec.equipment_models then
			-- Replacer equipment.
			slots:set_object{slot = slot}
			o.equipment = o.equipment or {}
			o.equipment[slot] = spec.name
			o:update_model()
		else
			-- Add-on equipment.
			slots:set_object{slot = slot, model = spec.model}
			if o.equipment and o.equipment[slot] then
				o.equipment[slot] = nil
				o:update_model()
			end
		end
		-- Torch holding hack.
		if slot == "hand.L" then
			if spec then
				o:animate{animation = "hold-left", channel = Animation.CHANNEL_LEFT_HAND, weight = 2,
					fade_in = 0.5, fade_out = 0.5, permanent = true}
			else
				o:animate{animation = "empty", channel = Animation.CHANNEL_LEFT_HAND}
			end
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SPEECH", func = function(event)
	local ok,i,m = event.packet:read("uint32", "string")
	if ok then
		local o = Object:find{id = i}
		if o then
			local n = o.name or "???"
			Speech:add{object = o, message = m}
			Gui.chat_history:append{text = "<" .. n .. "> " .. m}
			Sound:effect{object = o, effect = "spring-000"}
		end
	end
end}

Protocol:add_handler{type = "VOXEL_DIFF", func = function(event)
	Voxel:set_block{packet = event.packet}
end}

Protocol:add_handler{type = "WORLD_EFFECT", func = function(event)
	local ok,t,x,y,z = event.packet:read("string", "float", "float", "float")
	if ok then
		-- Find the effect.
		local effect = Effect:find{name = t}
		if not effect then return end
		-- Create the effect object.
		EffectObject{
			model = effect.model,
			position = Vector(x,y,z),
			sound = effect.sound,
			sound_pitch = effect.sound_pitch,
			realized = true}
	end
end}
