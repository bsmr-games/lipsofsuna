Widgets.Menus = Class(Widget)

Widgets.Menus.new = function(clss, args)
	local self = Widget.new(clss, {rows = 3, cols = 1, stack = {}})
	self.button_back = Widgets.MenuItem{text = "Back", pressed = function() self:back() end}
	self.button_close = Widgets.MenuItem{text = "Close", pressed = function() self:close() end}
	local group = Widget{rows = 1, cols = 2}
	group:set_child{col = 1, row = 1, widget = self.button_back}
	group:set_child{col = 2, row = 1, widget = self.button_close}
	self:set_child{col = 1, row = 2, widget = Widgets.Label{text = "_________________"}}
	self:set_child{col = 1, row = 3, widget = group}
	for k,v in pairs(args or {}) do self[k] = v end
	return self
end

Widgets.Menus.back = function(self)
	local w = self.stack[#self.stack]
	if w.back then
		w:back()
	else
		self:close{level = #self.stack}
	end
end

--- Opens a new menu level.
-- @param self Menus.
-- @oaram args Arguments.<ul>
--   <li>level: Activated menu level.</li>
--   <li>widget: Submenu to show.</li></ul>
Widgets.Menus.open = function(self, args)
	-- Close levels if opening from an upper level.
	if #self.stack >= args.level then
		self:close{hide = false, level = args.level}
	end
	-- Append the new widget.
	table.insert(self.stack, args.widget)
	args.widget.level = #self.stack
	self:set_child{row = 1, col = 1, widget = args.widget}
	-- Make sure we are visible.
	self.visible = true
end

--- Closes menu levels.
-- @param self Menus.
-- @oaram args Arguments.<ul>
--   <li>hide: False to not hide the menu even if it becomes empty.</li>
--   <li>level: Closed menu level or nil to close all.</li></ul>
Widgets.Menus.close = function(self, args)
	local h = not args or args.hide ~= false
	-- Pop levels from the stack.
	local l = math.max(1, args and args.level or 1)
	while #self.stack >= l do
		local w = self.stack[#self.stack]
		if w.close then w:close() end
		self.stack[#self.stack] = nil
	end
	-- Show the topmost level or hide if all levels closed.
	if #self.stack > 0 then
		self:set_child{row = 1, col = 1, widget = self.stack[#self.stack]}
	elseif h then
		self.visible = false
	end
	-- Return to the game mode when the menu is closed.
	if h and not self.visible then
		Client:set_mode("game")
	end
end

