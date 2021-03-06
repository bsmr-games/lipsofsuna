--- Character generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module character.chargen
-- @alias Chargen

local ChargenCamera = require("character/camera")
local Class = require("system/class")
local Color = require("system/color")
local Hooks = require("system/hooks")
local Json = require("system/json")
local PhysicsConsts = require("core/physics/consts")

local scale255 = function(t)
	local res = {}
	for k,v in pairs(t) do res[k] = v * 255 end
	return res
end

--- Character generator.
-- @type Chargen
local Chargen = Class("Chargen")

--- Creates a new character generator.
-- @param clss Chargen class.
-- @return Chargen.
Chargen.new = function(clss)
	local self = Class.new(clss)
	self.char = {}
	self.data = {}
	self.camera = ChargenCamera()
	return self
end

--- Initializes the character creator.
-- @param self Chargen.
-- @param standalone True to run in standalone mode.
Chargen.init = function(self, standalone)
	-- Initialize the state.
	self.data.standalone = standalone
	self.data.active = true
	-- Create the object.
	self.data.object = Main.objects:create_object_by_spec("Actor", "aer-player")
	self.data.object:set_position(Vector(1, 1, 1))
	self.data.object:randomize()
	self.data.object:set_visible(true)
	self.data.object.physics:set_physics("static")
	self.data.object.render:init(self.data.object)
	self.data.object.render:add_animation("idle")
	self.camera.object = self.data.object
	self:set_race("aer")
	self.data.update_needed = true
	-- Change the music track.
	Main.music_manager:switch_track("char", true)
end

--- Resets the character creator.
--
-- Context: Any.
--
-- @param self Chargen.
Chargen.reset = function(self)
	if self.data.object then self.data.object:set_visible(false) end
	self.data = {}
	self.char = {
		animation_profile = "default",
		body = {},
		eye_color = {1,1,1},
		eye_style = "aereye1",
		face = {},
		hair_color = {1,1,1},
		hair_style = "default",
		head_scale = 0.5,
		head_style = "aerhead1",
		height = 0.5,
		name = "Guest",
		race = "aer",
		skin_color = {1,1,1},
		skin_style = "",
		spawn_point = "Home"}
	-- Initialize the body sliders.
	local max_body = 0
	for k,v in ipairs(Main.specs:find_by_category("ChargenSliderSpec", "body")) do
		self.char.body[v.field_index] = v.default
		max_body = math.max(max_body, v.field_index)
	end
	for i = 1,max_body do
		if not self.char.body[i] then
			self.char.body[i] = 0
		end
	end
	-- Initialize the face sliders.
	local max_face = 0
	for k,v in ipairs(Main.specs:find_by_category("ChargenSliderSpec", "face")) do
		self.char.face[v.field_index] = v.default
		max_face = math.max(max_face, v.field_index)
	end
	for i = 1,max_face do
		if not self.char.face[i] then
			self.char.face[i] = 0
		end
	end
end

--- Sends the character to the server.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
Chargen.apply = function(self)
	if self.data.standalone then return end
	Main.messaging:client_event("create character", {
		animation_profile = self.char.animation_profile,
		body_scale = self.char.height,
		body_sliders = scale255(self.char.body),
		brow_style = self.char.brow_style,
		eye_color = scale255(self.char.eye_color),
		eye_style = self.char.eye_style,
		face_sliders = scale255(self.char.face),
		face_style = self.char.face_style,
		hair_color = scale255(self.char.hair_color),
		hair_style = self.char.hair_style,
		-- TODO: head_scale
		head_style = self.char.head_style,
		mouth_style = self.char.mouth_style,
		name = self.char.name,
		race = self.char.race,
		skin_color = scale255(self.char.skin_color),
		skin_style = self.char.skin_style,
		spawn_point = self.char.spawn_point})
end

--- Handles character generator controls.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param args Event arguments.
-- @return True if the caller should handle the event.
Chargen.input = function(self, args)
	if args.type == "mousepress" and args.button == 1 then
		self.data.drag = true
		return false
	elseif args.type == "mouserelease" and args.button == 1 then
		self.data.drag = nil
		return false
	elseif args.type == "mousemotion" and self.data.drag then
		local sens = Client.options.mouse_sensitivity * 0.02
		Client.camera_manager:turn(-args.dx * sens)
		if Client.options.invert_mouse then sens = -sens end
		Client.camera_manager:tilt(args.dy * sens)
		return false
	elseif args.type == "mousescroll" then
		Client.camera_manager:zoom(args.rel < 0 and -1 or 1)
		return false
	end
	return true
end

--- Randomizes the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
Chargen.randomize = function(self)
	self:set_race("aer")
	self.data.update_needed = true
end

--- Saves the current character.
-- @param self Chargen.
Chargen.save = function(self)
	-- Set the simple fields.
	local preset = {
		type = "ActorPresetSpec",
		animation_profile = self.char.animation_profile,
		body = {},
		eye_color = self.char.eye_color,
		eye_style = self.char.eye_style,
		face = {},
		hair_color = self.char.hair_color,
		hair_style = self.char.hair_style,
		head_style = self.char.head_style,
		skin_color = self.char.skin_color,
		skin_style = self.char.skin_style}
	-- Set the body slider states.
	for k,v in ipairs(Main.specs:find_by_category("ChargenSliderSpec", "body")) do
		local val = self.char.body[v.field_index]
		if val then
			preset.body[v.name] = val
		end
	end
	-- Set the face slider states.
	for k,v in ipairs(Main.specs:find_by_category("ChargenSliderSpec", "face")) do
		local val = self.char.face[v.field_index]
		if val then
			preset.face[v.name] = val
		end
	end
	-- TODO: Save to a file
	print(Json:encode(preset))
end

--- Updates the character generator.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param secs Seconds since the last update.
Chargen.update = function(self, secs)
	if not self.data.active then return end
	self.data.object:refresh(10)
	Client.camera_manager:set_camera_mode("chargen")
	-- Build models and textures.
	if self.data.update_needed then
		self.data.update_needed = nil
		local spec = Main.specs:find_by_name("ActorSpec", self.char.race .. "-player")
		local data = self:get_build_data()
		self.data.object:set_spec(spec)
		self.data.object.physics:set_collision_group(PhysicsConsts.GROUP_PLAYERS)
		self.data.object.body_scale = self.char.height
		self.data.object.body_sliders = scale255(self.char.body)
		self.data.object.brow_style = self.char.brow_style
		self.data.object.eye_color = scale255(self.char.eye_color)
		self.data.object.eye_style = self.char.eye_style
		self.data.object.face_sliders = scale255(self.char.face)
		self.data.object.face_style = self.char.face_style
		self.data.object.hair_color = scale255(self.char.hair_color)
		self.data.object.hair_style = self.char.hair_style
		self.data.object.head_scale = self.char.head_scale
		self.data.object.head_style = self.char.head_style
		self.data.object.mouth_style = self.char.mouth_style
		self.data.object.skin_color = scale255(self.char.skin_color)
		self.data.object.skin_style = self.char.skin_style
		self.data.object.render:request_model_rebuild()
	end
end

--- Gets the animation profile of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Profile name.
Chargen.get_animation_profile = function(self)
	return self.char.animation_profile
end

--- Sets the animation profile of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Profile name.
Chargen.set_animation_profile = function(self, value)
	self.char.animation_profile = value
	self.data.object.animation_profile = value
	self.data.object.render:add_animation("idle")
end

--- Gets the value of a body slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param slider Slider index.
-- @return Slider value.
Chargen.get_body = function(self, slider)
	return self.char.body[slider]
end

--- Sets the value of a body slider.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param slider Slider index.
-- @param value Slider value.
Chargen.set_body = function(self, slider, value)
	self.char.body[slider] = value
	self.data.update_needed = true
end

--- Gets the brow style of the character.
-- @param self Chargen.
-- @return Style name.
Chargen.get_brow_style = function(self)
	return self.char.brow_style
end

--- Sets the face style of the character.
-- @param self Chargen.
-- @param value Style name.
Chargen.set_brow_style = function(self, value)
	self.char.brow_style = value
	self.data.update_needed = true
end

--- Gets the data form model and texture builders.
-- @param self Chargen.
-- @return Table.
Chargen.get_build_data = function(self)
	local spec = Main.specs:find_by_name("ActorSpec", self.char.race .. "-player")
	return {
		beheaded = false,
		body_scale = self.char.height,
		body_sliders = scale255(self.char.body),
		brow_style = self.char.brow_style,
		equipment = {"bloomers", "bloomers top"},
		eye_color = self.char.eye_color,
		eye_style = self.char.eye_style,
		face_sliders = scale255(self.char.face),
		face_style = self.char.face_style,
		hair_color = self.char.hair_color,
		head_scale = self.char.head_scale,
		hair_style = self.char.hair_style,
		head_style = self.char.head_style,
		mouth_style = self.char.head_style,
		nudity = Client.options.nudity_enabled,
		skin_color = self.char.skin_color,
		skin_style = self.char.skin_style,
		spec = spec}
end

--- Gets the camera focus position.
-- @param self Chargen.
-- @return Vector if active. Nil otherwise.
Chargen.get_camera_focus = function(self)
	if not self.data.active then return end
	return self.data.object:get_position()
end

--- Gets the eye style of the character.
-- @param self Chargen.
-- @return Style name.
Chargen.get_eye_style = function(self)
	return self.char.eye_style
end

--- Sets the eye style of the character.
-- @param self Chargen.
-- @param value Style name.
Chargen.set_eye_style = function(self, value)
	self.char.eye_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the eye color.
-- @param self Chargen.
-- @param channel Channel index.
-- @return Channel value.
Chargen.get_eye_color = function(self, channel)
	return self.char.eye_color[channel]
end

--- Sets one of the HSV channels of the eye color.
-- @param self Chargen.
-- @param channel Channel index.
-- @param value Channel value.
Chargen.set_eye_color = function(self, channel, value)
	self.char.eye_color[channel] = value
	self.data.update_needed = true
end

--- Gets the value of a face slider.
-- @param self Chargen.
-- @param slider Slider index.
-- @return Slider value.
Chargen.get_face = function(self, slider)
	return self.char.face[slider]
end

--- Sets the value of a face slider.
-- @param self Chargen.
-- @param slider Slider index.
-- @param value Slider value.
Chargen.set_face = function(self, slider, value)
	self.char.face[slider] = value
	self.data.update_needed = true
end

--- Gets the face style of the character.
-- @param self Chargen.
-- @return Style name.
Chargen.get_face_style = function(self)
	return self.char.face_style
end

--- Sets the face style of the character.
-- @param self Chargen.
-- @param value Style name.
Chargen.set_face_style = function(self, value)
	self.char.face_style = value
	self.data.update_needed = true
end

--- Gets the hair style of the character.
-- @param self Chargen.
-- @return Style name.
Chargen.get_hair_style = function(self)
	return self.char.hair_style
end

--- Sets the hair style of the character.
-- @param self Chargen.
-- @param value Style name.
Chargen.set_hair_style = function(self, value)
	self.char.hair_style = value
	self.data.object.hair_style = value
end

--- Gets one of the HSV channels of the hair color.
-- @param self Chargen.
-- @param channel Channel index.
-- @return Channel value.
Chargen.get_hair_color = function(self, channel)
	return self.char.hair_color[channel]
end

--- Sets one of the HSV channels of the hair color.
-- @param self Chargen.
-- @param channel Channel index.
-- @param value Channel value.
Chargen.set_hair_color = function(self, channel, value)
	self.char.hair_color[channel] = value
	self.data.object.hair_color = scale255(self.char.hair_color)
end

--- Gets the head style of the character.
-- @param self Chargen.
-- @return Style name.
Chargen.get_head_style = function(self)
	return self.char.head_style
end

--- Gets the head scaling of the character.
-- @param self Chargen.
-- @return Scaling.
Chargen.get_head_scale = function(self, value)
	return self.char.head_scale
end

--- Sets the head scaling of the character.
-- @param self Chargen.
-- @param value Scaling.
Chargen.set_head_scale = function(self, value)
	self.char.head_scale = value
	self.data.object.head_scale = self.char.head_scale
	self.data.object.render:update_scale()
end

--- Sets the head style of the character.
-- @param self Chargen.
-- @param value Style name.
Chargen.set_head_style = function(self, value)
	self.char.head_style = value
	self.data.update_needed = true
end

--- Gets the height of the character.
-- @param self Chargen.
-- @return Height.
Chargen.get_height = function(self, value)
	return self.char.height
end

--- Sets the height of the character.
-- @param self Chargen.
-- @param value Height.
Chargen.set_height = function(self, value)
	self.char.height = value
	self.data.object.body_scale = self.char.height
	self.data.object.render:update_scale()
end

--- Gets the mouth style of the character.
-- @param self Chargen.
-- @return Style name.
Chargen.get_mouth_style = function(self)
	return self.char.mouth_style
end

--- Sets the mouth style of the character.
-- @param self Chargen.
-- @param value Style name.
Chargen.set_mouth_style = function(self, value)
	self.char.mouth_style = value
	self.data.update_needed = true
end

--- Gets the name of the character.
-- @param self Chargen.
-- @return Name.
Chargen.get_name = function(self)
	return self.char.name
end

--- Sets the name of the character.
-- @param self Chargen.
-- @param value Name.
Chargen.set_name = function(self, value)
	self.char.name = value
end

--- Sets the character from a preset.
-- @param self Chargen.
-- @param spec Preset spec.
Chargen.set_preset = function(self, spec)
	if not spec then return end
	for k,v in pairs(spec:get_chargen()) do
		if type(v) == "table" then
			local t = {}
			for k1,v1 in pairs(v) do t[k1] = v1 end
			self.char[k] = t
		else
			self.char[k] = v
		end
	end
	if spec.animation_profile then
		self.data.object.animation_profile = spec.animation_profile
	else
		self.char.animation_profile = "default"
		self.data.object.animation_profile = nil
	end
	self.data.object.render:add_animation("idle")
	self.data.update_needed = true
end

--- Gets the list of available presets.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Indexed list of presets.
Chargen.get_presets = function(self)
	local presets = {}
	for k,v in pairs(Main.specs:get_spec_names("ActorPresetSpec")) do
		if v.playable then
			table.insert(presets, v)
		end
	end
	table.sort(presets, function(a,b) return a.name < b.name end)
	return presets
end

--- Gets the name of the active race.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Race name.
Chargen.get_race = function(self)
	return self.char.race
end

--- Sets the name of the active race.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param race Race name.
Chargen.set_race = function(self, race)
	-- Set the race selection.
	self.char.race = race
	-- Choose a random preset.
	local presets = self:get_presets()
	local preset = presets[math.random(1,#presets)]
	self:set_preset(preset)
end

--- Gets the skin style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Skin style name.
Chargen.get_skin_style = function(self)
	return self.char.skin_style
end

--- Sets the skin style of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Style name.
Chargen.set_skin_style = function(self, value)
	self.char.skin_style = value
	self.data.update_needed = true
end

--- Gets one of the HSV channels of the skin color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @return Channel value.
Chargen.get_skin_color = function(self, channel)
	return self.char.skin_color[channel]
end

--- Sets one of the HSV channels of the skin color.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param channel Channel index.
-- @param value Channel value.
Chargen.set_skin_color = function(self, channel, value)
	self.char.skin_color[channel] = value
	self.data.update_needed = true
end

--- Gets the spawn point of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @return Spawn point name.
Chargen.get_spawn_point = function(self, channel)
	return self.char.spawn_point
end

--- Sets the spawn point of the character.
--
-- Context: The character creator must have been initialized.
--
-- @param self Chargen.
-- @param value Spawn point name.
Chargen.set_spawn_point = function(self, value)
	self.char.spawn_point = value
end

return Chargen
