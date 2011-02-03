Widgets.Menu = Class(Widget)

Widgets.Menu.new = function(clss, args)
	local self = Widget.new(clss)
	self.cols = 1
	-- Copy arguments.
	for k,v in pairs(args or {}) do
		if type(k) ~= "number" then
			self[k] = v
		end
	end
	-- Create menu items.
	for k,v in ipairs(args or {}) do
		if type(v[2]) == "function" then
			self:append{text = v[1], pressed = v[2], arrow = v.arrow}
		else
			self:append{text = v[1], widget = v[2], arrow = true}
		end
	end
	return self
end

Widgets.Menu.append = function(self, args)
	local row = self.rows + 1
	local button = Widgets.MenuItem{font = "medium", text = args.text, arrow = args.arrow,
	pressed = function()
		if args.widget then Gui.menus:open{level = self.level + 1, widget = args.widget} end
		if args.pressed then args.pressed(self, row) end
	end}
	self:append_row(button)
end
