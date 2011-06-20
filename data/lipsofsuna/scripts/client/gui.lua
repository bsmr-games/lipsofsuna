Gui = Class()
Gui.admin = true

--- Initializes the in-game user interface.
-- @param clss Gui class.
Gui.init = function(clss)
	Gui.skills = Views.Skills{main = true, sync = true}
	-- Admin menu.
	Gui.menu_widget_admin = Widgets.Menu{text = "Admin",
		{"Stats", function() clss:set_mode("admin", 2) end, arrow = true},
		{"Editor", Editing.dialog},
		{"Save", function() Network:send{packet = Packet(packets.ADMIN_SAVE)} end},
		{"Shutdown", function() Network:send{packet = Packet(packets.ADMIN_SHUTDOWN)} end}}
	Gui.menu_widget_main = Widgets.Menu{text = "Menu",
		{"Inventory", function() clss:set_mode("inventory", 2) end, arrow = true},
		{"Quests", function() clss:set_mode("quests", 2) end, arrow = true},
		{"Skills", function() clss:set_mode("skills", 2) end, arrow = true},
		{"Spells", function() clss:set_mode("feats", 2) end, arrow = true},
		{"-----", function() end},
		{"Controls", function() clss:set_mode("controls", 2) end, arrow = true},
		{"Options", function() clss:set_mode("options", 2) end, arrow = true},
		{"Admin", Gui.menu_widget_admin},
		{"Help", function() clss:set_mode("help", 2) end, arrow = true},
		{"Quit", function() Program.quit = true end}}
	Gui.menus = Widgets.Menus()
	Gui.chat_history = Widgets.Log()
	-- Skill group.
	Gui.skill_health = Widgets.SkillControl{compact = true}
	Gui.skill_mana = Widgets.SkillControl{compact = true}
	local skillgrp = Widget{cols = 1, spacings = {0,0}}
	skillgrp:append_row(Gui.skill_health)
	skillgrp:append_row(Gui.skill_mana)
	-- Chat entry.
	Gui.chat_label = Widgets.Label{text = " "}
	Gui.chat_entry = Widgets.Entry{transparent = true, visible = false}
	Gui.chat_entry.pressed = function(self)
		Network:send{packet = Packet(packets.PLAYER_CHAT, "string", self.text)}
		self:clear()
	end
	Gui.chat_group = Widget{cols = 1, spacings = {0,0}}
	Gui.chat_group:append_row(Gui.chat_history)
	Gui.chat_group:set_expand{col = 1, row = 1}
	Gui.chat_group = Widget{cols = 2, rows = 1, spacings = {0, 0}}
	Gui.chat_group:set_child(1, 1, Gui.chat_label)
	Gui.chat_group:set_child(2, 1, Gui.chat_entry)
	Gui.chat_group:set_expand{col = 2}
	-- Skills group.
	local pad = Widget()
	pad:set_request{width = 64}
	Gui.fps_label = Widgets.Label{valign = 1}
	Gui.skills_group = Widgets.Frame{cols = 4, rows = 2, style = "quickbar"}
	Gui.skills_group:set_child{col = 1, row = 2, widget = skillgrp}
	Gui.skills_group:set_child{col = 2, row = 2, widget = pad}
	Gui.skills_group:set_child{col = 3, row = 1, widget = Gui.chat_group}
	Gui.skills_group:set_child{col = 3, row = 2, widget = Quickslots.group}
	Gui.skills_group:set_child{col = 4, row = 2, widget = Gui.fps_label}
	Gui.skills_group:set_expand{col = 3}
	-- Modifiers.
	Gui.modifiers = Widgets.Modifiers()
	-- Respawning.
	Gui.button_respawn = Widgets.Button{font = "medium", text = "Create a new character",
		pressed = function() Network:send{packet = Packet(packets.PLAYER_RESPAWN)} end}
	Gui.group_respawn = Widget{cols = 1, rows = 3, margins = {0,0,5,0}, visible = false}
	Gui.group_respawn:set_expand{col = 1, row = 1}
	Gui.group_respawn:set_expand{row = 3}
	Gui.group_respawn:set_child(1, 1, Gui.button_respawn)
	Gui.group_respawn:set_child(1, 2, Gui.button_respawn)
	-- Dialog.
	Gui.group_dialog = Widget{cols = 1, rows = 3}
	Gui.group_dialog:set_expand{row = 1}
	Gui.group_dialog:set_child(1, 2, Widgets.Label())
	-- Packing.
	Gui.group_top = Widget{cols = 3, rows = 1}
	Gui.group_top:set_expand{col = 1}
	Gui.group_top:set_child(1, 1, Gui.chat_history)
	Gui.group_top:set_child(2, 1, Gui.modifiers)
	Gui.group_top:set_child(3, 1, Gui.group_respawn)
	Gui.group_middle = Widget{cols = 2, rows = 1}
	Gui.group_middle:set_expand{col = 1, row = 1}
	Gui.group_middle:set_child(1, 1, Gui.menus)
	Gui.group_middle:set_child(2, 1, Gui.group_dialog)
	Gui.scene = Widgets.Scene{cols = 1, rows = 3, camera = Player.camera, margins = {5,5,0,0}, spacings = {0,0}}
	Gui.scene:set_expand{col = 1, row = 1}
	Gui.scene:set_child(1, 1, Gui.group_top)
	Gui.scene:set_child(1, 2, Gui.group_middle)
	Gui.scene:set_child(1, 3, Gui.skills_group)
	Gui.main = Widget{cols = 1, behind = true, fullscreen = true}
	Gui.main:append_row(Gui.scene)
	Gui.main:set_expand{col = 1, row = 1}
end

Gui.set_admin = function(self, admin)
	if admin == self.admin then return end
	Gui.menu_widget_main:replace(8, admin and {"Admin", Gui.menu_widget_admin})
end

Gui.set_dead = function(self, value)
	if self.group_respawn.visible == value then return end
	if value then
		self.modifiers.visible = false
		self.group_respawn.visible = true
	else
		self.group_respawn.visible = false
		self.modifiers.visible = true
	end
end

Gui.set_mode = function(self, mode, level)
	-- Check for state changes.
	if self.mode == mode then return end
	-- Close the old view.
	Target:cancel()
	Drag:cancel()
	if self.view and self.view.close then self.view:close() end
	if Widgets.ComboBox.active then Widgets.ComboBox.active:close() end
	-- Set the base view mode.
	local base = (mode == "startup" or mode == "wait") and 1 or (mode == "chargen") and 2 or 3
	if base ~= self.mode_base then
		if self.mode_base == 3 then
			-- Disconnecting.
			for k,v in pairs(Object.objects) do
				v.realized = false
				Player.object = nil
			end
			self.main.floating = false
		end
		if base == 1 then
			-- Connection mode.
			self.mode_base = 1
			Startup.group.floating = true
			Sound:switch_music_track("menu")
		elseif mode == "chargen" then
			-- Character creation.
			self.mode_base = 2
			Views.Chargen.floating = true
			Sound:switch_music_track("char")
		else
			-- Game modes.
			self.mode_base = 3
			self.main.floating = true
			Program:unload_world()
			Sound:switch_music_track("game")
		end
	end
	-- Maintain dialog visibility.
	-- Dialogs don't fit most of the time so only show them in game and menu modes.
	if mode == "game" or mode == "menu" then
		self.group_dialog.visible = true
	else
		self.group_dialog.visible = false
	end
	-- Set the detailed view mode.
	self.mode = mode
	if mode == "admin" then
		Gui.menus:open{level = level or 1, widget = Views.Admin.inst}
		self.view = Views.Admin.inst
	elseif mode == "book" then
		Gui.menus:open{level = level or 1, widget = Views.Book.inst}
		self.view = Views.Book.inst
	elseif mode == "chargen" then
		self.view = Views.Chargen.inst
	elseif mode == "controls" then
		Gui.menus:open{level = level or 1, widget = Views.Controls.inst}
		self.view = Views.Controls.inst
	elseif mode == "feats" then
		Gui.menus:open{level = level or 1, widget = Views.Feats.inst}
		self.view = Views.Feats.inst
	elseif mode == "game" then
		Gui.menus:close()
		self.view = Views.Game.inst
	elseif mode == "help" then
		Gui.menus:open{level = level or 1, widget = Views.Help.inst}
		self.view = Views.Help.inst
	elseif mode == "inventory" then
		Gui.menus:open{level = level or 1, widget = Views.Inventory.inst}
		self.view = Views.Inventory.inst
	elseif mode == "menu" then
		Gui.menus:open{level = level or 1, widget = Gui.menu_widget_main}
		self.view = Gui.menu_widget_main
	elseif mode == "options" then
		Gui.menus:open{level = level or 1, widget = Views.Options.inst}
		self.view = Views.Options.inst
	elseif mode == "quests" then
		Gui.menus:open{level = level or 1, widget = Views.Quests.inst}
		self.view = Views.Quests.inst
	elseif mode == "skills" then
		Gui.menus:open{level = level or 1, widget = self.skills}
		self.view = self.skills
	elseif mode == "startup" then
		self.view = Startup
	end
	if self.view and self.view.enter then self.view:enter() end
end

--- Sets or unsets the text of the action label.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_action_text = function(clss, text)
	clss.scene.action = text
end

--- Sets the ID of the object whose dialog is shown.
-- @param self Gui class.
-- @param id Object ID.
Gui.set_dialog = function(clss, id)
	-- Find the dialog.
	clss.active_dialog = id
	local obj = id and Object:find{id = id}
	local dlg = obj and obj.dialog
	-- Update the dialog UI.
	if not dlg then
		-- Hide the dialog UI.
		clss.dialog_choices = nil
		clss.group_dialog.rows = 1
		clss.group_dialog.visible = false
	elseif dlg.type == "choice" then
		-- Show a dialog choice.
		clss.dialog_choices = {}
		clss.group_dialog.rows = 1
		for k,v in ipairs(dlg.choices) do
			local widget = Widgets.DialogLabel{choice = true, index = k, text = v, pressed = function()
				Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", v)}
			end}
			table.insert(clss.dialog_choices, widget)
			clss.group_dialog:append_row(widget)
		end
		clss.group_dialog.visible = true
	else
		-- Show a dialog line.
		local widget = Widgets.DialogLabel{index = 1, text = dlg.message, pressed = function()
			Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", "")}
		end}
		clss.dialog_choices = {widget}
		clss.group_dialog.rows = 1
		clss.group_dialog:append_row(widget)
		clss.group_dialog.visible = true
	end
end

--- Sets or unsets the active target.
-- @param clss Gui class.
-- @param text String or nil.
Gui.set_target_text = function(clss, text)
	clss.scene.action = text
end

Eventhandler{type = "tick", func = function(self, args)
	if Gui.fps_label then
		Gui.fps_label.text = "FPS: " .. math.floor(Client.fps + 0.5)
	end
end}

Gui.class_getters = {
	chat_active = function(s) return s.chat_entry.visible end}

Gui.class_setters = {
	chat_active = function(s, v)
		s.chat_entry.visible = v
		s.chat_label.text = v and "TALK: " or " "
		Gui:set_mode("game")
	end}
