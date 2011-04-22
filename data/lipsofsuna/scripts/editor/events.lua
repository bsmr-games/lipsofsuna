local gui_actions = {menu = true, screenshot = true}

Eventhandler{type = "keypress", func = function(self, args)
	if not Client.moving then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	if not Client.moving then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mousepress", func = function(self, args)
	if not Client.moving then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	if not Client.moving then
		Widgets:handle_event(args)
		Action:event(args, gui_actions)
	else
		Action:event(args)
	end
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

Eventhandler{type = "tick", func = function(self, args)
	Editor.inst.camera:update(args.secs)
	Editor.inst.light.position = Editor.inst.camera.target_position +
		Editor.inst.camera.rotation * Vector(0,0,-5)
	-- Update the cursor.
	Widgets.Cursor.inst:update()
end}
