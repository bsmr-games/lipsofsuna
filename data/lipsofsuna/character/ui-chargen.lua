local Client = require("core/client/client")

local chargen_input = function(args)
	return Client.chargen:input(args)
end

local chargen_update = function(secs)
	Client.chargen:update(secs)
end

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen",
	root = "chargen",
	label = "Create character",
	init = function() Client.lighting:set_dungeon_mode(false) end,
	exit_root = function() Client.chargen:reset() end,
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen",
	widget = function()
		local widget = Widgets.Uientry("Name", function(w)
			Client.chargen:set_name(w.value)
		end)
		widget.value = Client.chargen:get_name()
		return widget
	end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Race", "chargen/race") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Preset", "chargen/presets") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Body", "chargen/body") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Head", "chargen/head") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Face", "chargen/face") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Misc", "chargen/misc") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Start point", "chargen/spawnpoint") end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uibutton("Create", function()
			Client.chargen:apply()
		end)
	end}

Ui:add_widget{
	state = "chargen",
	widget = function() return Widgets.Uitransition("Quit", "quit") end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/race",
	label = "Select race",
	init = function()
		-- Create the race radio buttons.
		local widgets = {}
		local races = Client.chargen:get_races()
		for k,v in ipairs(races) do
			local widget = Widgets.Uiradio(v[1], "race", function(w)
				Client.chargen:set_race(w.race)
			end)
			widget.race = v[2]
			if widget.race == Client.chargen:get_race() then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/presets",
	label = "Select preset",
	init = function()
		-- Create the preset radio buttons.
		local widgets = {}
		local presets = Client.chargen:get_presets()
		for k,v in ipairs(presets) do
			local widget = Widgets.Uiradio(v.name, "preset", function(w)
				Client.chargen:set_preset(w.preset)
			end)
			widget.preset = v
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/presets",
	widget = function() return Widgets.Uibutton("Save the current character", function()
			Client.chargen:save()
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/body",
	label = "Customize body",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/body",
	widget = function() return Widgets.Uitransition("Skin style", "chargen/body/skinstyle") end}

Ui:add_widget{
	state = "chargen/body",
	widget = function()
		-- Create the body proportion sliders.
		local widgets = {}
		for k,v in ipairs(ChargenSliderSpec:find_by_category("body")) do
			local value = Client.chargen:get_body(v.field_index) or 0
			local widget = Widgets.Uiscrollfloat(v.name, 0, 1, value, function(w)
				Client.chargen:set_body(k, w.value)
			end)
			table.insert(widgets, widget)
		end
		return widgets
	end}

Ui:add_widget{
	state = "chargen/body",
	widget = function()
		local value = Client.chargen:get_height()
		return Widgets.Uiscrollfloat("Height", 0, 1, value, function(w)
			Client.chargen:set_height(w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/body/skinstyle",
	label = "Select skin style",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local specs = Actorskinspec:find_by_actor(race)
		for k,v in ipairs(specs) do
			local widget = Widgets.Uiradio(v.name, "skin", function(w)
				Client.chargen:set_skin_style(w.style)
			end)
			widget.style = v.name
			if Client.chargen:get_skin_style() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/head",
	label = "Customize head",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/head",
	widget = function() return Widgets.Uitransition("Head style", "chargen/head/headstyle") end}

Ui:add_widget{
	state = "chargen/head",
	widget = function() return Widgets.Uitransition("Hair style", "chargen/head/hairstyle") end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		local value = Client.chargen:get_hair_color(1)
		return Widgets.Uiscrollfloat("Hair hue", 0, 1, value, function(w)
			Client.chargen:set_hair_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		local value = Client.chargen:get_hair_color(2)
		return Widgets.Uiscrollfloat("Hair saturation", 0, 1, value, function(w)
			Client.chargen:set_hair_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/head",
	widget = function()
		local value = Client.chargen:get_hair_color(3)
		return Widgets.Uiscrollfloat("Hair lightness", 0, 1, value, function(w)
			Client.chargen:set_hair_color(3, w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/head/hairstyle",
	label = "Select hair style",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local spec = Actorspec:find{name = race}
		if not spec.hair_styles then return end
		local lst = {}
		for k,v in pairs(spec.hair_styles) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		for k,v in ipairs(lst) do
			local widget = Widgets.Uiradio(v[1], "hair", function(w)
				Client.chargen:set_hair_style(w.style)
			end)
			widget.style = v[2]
			if Client.chargen:get_hair_style() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/head/headstyle",
	label = "Select head style",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local spec = Actorspec:find{name = race}
		if not spec.head_styles then return end
		local lst = {}
		for k,v in pairs(spec.head_styles) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		for k,v in ipairs(lst) do
			local widget = Widgets.Uiradio(v[1], "head", function(w)
				Client.chargen:set_head_style(w.style)
			end)
			widget.style = v[2]
			if Client.chargen:get_head_style() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/face",
	label = "Customize face",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/face",
	widget = function() return Widgets.Uitransition("Eye style", "chargen/face/eyestyle") end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		local value = Client.chargen:get_eye_color(1)
		return Widgets.Uiscrollfloat("Eye hue", 0, 1, value, function(w)
			Client.chargen:set_eye_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		local value = Client.chargen:get_eye_color(2)
		return Widgets.Uiscrollfloat("Eye saturation", 0, 1, value, function(w)
			Client.chargen:set_eye_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		local value = Client.chargen:get_eye_color(3)
		return Widgets.Uiscrollfloat("Eye lightness", 0, 1, value, function(w)
			Client.chargen:set_eye_color(3, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/face",
	widget = function()
		-- Create the face shape sliders.
		local widgets = {}
		for k,v in ipairs(ChargenSliderSpec:find_by_category("face")) do
			local value = Client.chargen:get_face(v.field_index) or 0
			local widget = Widgets.Uiscrollfloat(v.name, 0, 1, value, function(w)
				Client.chargen:set_face(k, w.value)
			end)
			table.insert(widgets, widget)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/face/eyestyle",
	label = "Select eye style",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local spec = Actorspec:find{name = race}
		if not spec.head_styles then return end
		local lst = {}
		for k,v in pairs(spec.eye_styles) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		for k,v in ipairs(lst) do
			local widget = Widgets.Uiradio(v[1], "eye", function(w)
				Client.chargen:set_eye_style(w.style)
			end)
			widget.style = v[2]
			if Client.chargen:get_eye_style() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc",
	label = "Customize other",
	input_post = chargen_input,
	update = chargen_update}

Ui:add_widget{
	state = "chargen/misc",
	widget = function() return Widgets.Uitransition("Animation profile", "chargen/misc/animation") end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(1)
		return Widgets.Uiscrollfloat("Skin hue", 0, 1, value, function(w)
			Client.chargen:set_skin_color(1, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(2)
		return Widgets.Uiscrollfloat("Skin saturation", 0, 1, value, function(w)
			Client.chargen:set_skin_color(2, w.value)
		end)
	end}

Ui:add_widget{
	state = "chargen/misc",
	widget = function()
		local value = Client.chargen:get_skin_color(3)
		return Widgets.Uiscrollfloat("Skin lightness", 0, 1, value, function(w)
			Client.chargen:set_skin_color(3, w.value)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/misc/animation",
	label = "Select animation",
	init = function()
		local widgets = {}
		local race = Client.chargen:get_race()
		local spec = Actorspec:find{name = race}
		if not spec.animations then return end
		local lst = {}
		for k,v in pairs(spec.animations) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		for k,v in ipairs(lst) do
			local widget = Widgets.Uiradio(v[1], "animations", function(w)
				Client.chargen:set_animation_profile(w.profile)
			end)
			widget.profile = v[2]
			if Client.chargen:get_animation_profile() == widget.style then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}

------------------------------------------------------------------------------

Ui:add_state{
	state = "chargen/spawnpoint",
	label = "Select start point",
	init = function()
		-- Get the spawn point list.
		local spawnpoints = {}
		for k,v in ipairs(Patternspec:find_spawn_points()) do
			table.insert(spawnpoints, v.name)
		end
		table.sort(spawnpoints, function(a,b) return a < b end)
		table.insert(spawnpoints, 1, "Home")
		-- Create the widgets.
		local current = Client.chargen:get_spawn_point()
		local widgets = {}
		for k,v in ipairs(spawnpoints) do
			local widget = Widgets.Uiradio(v, "spawn", function(w)
				Client.chargen:set_spawn_point(w.spawnpoint)
			end)
			widget.spawnpoint = v
			if widget.spawnpoint == current then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end,
	input_post = chargen_input,
	update = chargen_update}
