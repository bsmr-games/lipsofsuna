local Class = require("system/class")
local Input = require("system/input")
local Widget = require("system/widget")

local Cursor = Class("Cursor", Widget)

Cursor.new = function(clss, cursor)
	local self = Widget.new(clss)
	self.cursor = cursor
	self:set_depth(10)
	return self
end

Cursor.update = function(self)
	-- Update position.
	local c = Input:get_pointer_position()
	self:set_x(c.x + 1)
	self:set_y(c.y + 1)
	-- Show tooltips.
	if self.tooltip then
		self.tooltip:set_floating(false)
		self.tooltip = nil
	end
	if self.widget then return end
	if self:get_floating() then
		local w = Widgets:find_handler_widget("tooltip")
		if w and w.tooltip then
			self.tooltip = w.tooltip
			w.tooltip:popup(c + Vector(5,5))
		end
	end
end

Cursor.reshaped = function(self)
	local icon = self.icon
	local cursor = self.cursor
	self:set_request(
		cursor and cursor.size[2] or 16,
		cursor and cursor.size[1] or 16, true)
	self:canvas_clear()
	if self.cursor then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = cursor.size,
			source_image = cursor.image,
			source_position = cursor.offset,
			source_tiling = {0,cursor.size[1],0,0,cursor.size[1],0}}
	end
end

return Cursor
