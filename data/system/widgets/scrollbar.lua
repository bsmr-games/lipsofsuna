--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.widgets.scrollbar
-- @alias Scrollbar

local Class = require("system/class")
local Input = require("system/input")
local Widget = require("system/widget")

--- TODO:doc
-- @type Scrollbar
local Scrollbar = Class("Scrollbar", Widget)

Scrollbar.new = function(clss)
	local self = Widget.new(clss)
	self.max = 0
	self.page = 1
	self.step = 1
	self.scroll_offset = 1
	return self
end

Scrollbar.changed = function(self, value)
end

Scrollbar.grabbed = function(self, value)
end

Scrollbar.get_value_at = function(self, point)
	if self.max == 0 then return end
	local p = point - Vector(self:get_x(), self:get_y())
	local v = p.y / self:get_height() * self.max
	return math.floor(v + 0.5)
end

Scrollbar.set_value_at = function(self, point)
	-- Ignore if the scroll range is empty.
	if self.max <= self.page then return end
	-- Clamp clicks on the buttons.
	local c = point - Vector(self:get_x(), self:get_y())
	c.y = math.max(c.y, 21)
	c.y = math.min(c.y, self:get_height() - 21 - 1)
	-- Scroll to the cursor.
	local v = self:get_value_at(point)
	if not v then return end
	self.scroll_offset = math.max(0, math.min(v - math.floor(self.page/2), self.max - self.page))
	self:reshaped()
	self:changed(self.scroll_offset)
	return true
end

Scrollbar.handle_event = function(self, args)
	if args.type == "mousemotion" then
		if Input:get_mouse_button_state() % 2 == 1 then
			self:set_value_at(Vector(args.x, args.y))
			return
		end
	elseif args.type == "mousepress" then
		if args.button ~= 1 then return end
		if self.max <= self.page then return end
		local cursor = Input:get_pointer_position()
		local c = cursor - Vector(self:get_x(), self:get_y())
		if c.y < 21 then
			self:scroll("up")
		elseif c.y >= self:get_height() - 21 then
			self:scroll("down")
		else
			self:set_value_at(cursor)
			self:grabbed(true)
		end
		return
	elseif args.type == "mouserelease" then
		if args.button ~= 1 then return end
		self:grabbed(false)
		return
	elseif args.type == "mousescroll" then
		if args.rel > 0 then
			self:scroll("up")
		else
			self:scroll("down")
		end
		return
	end
	return true
end

Scrollbar.reshaped = function(self)
	self:set_request(21, 64, true)
	local w = self:get_width()
	local h = self:get_height()
	-- Draw the background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,21},
		dest_size = {w,h-42},
		source_image = "widgets2",
		source_position = {700,0},
		source_tiling = {7,86,7,7,86,7}}
	-- Draw the slider.
	local sy,sh
	local alloc = h - 42
	if self.max > 0 then
		sy = alloc * self.scroll_offset / self.max
		sh = math.min(alloc, math.max(20, alloc * self.page / self.max))
	else
		sy = 0
		sh = alloc
	end
	self:canvas_image{
		dest_position = {0,21 + sy},
		dest_size = {w,sh},
		source_image = "widgets2",
		source_position = {100,100},
		source_tiling = {7,86,7,7,86,7}}
	-- Draw the buttons.
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {21,21},
		source_image = "widgets2",
		source_position = (self.scroll_offset > 0) and {600,0} or {600,200},
		source_tiling = {7,86,7,7,86,7}}
	self:canvas_image{
		dest_position = {0,h-22},
		dest_size = {21,21},
		source_image = "widgets2",
		source_position = (self.scroll_offset < self.max - self.page) and {600,100} or {600,300},
		source_tiling = {7,86,7,7,86,7}}
	self:canvas_compile()
end

Scrollbar.scroll = function(self, dir)
	if dir == "up" then
		self.scroll_offset = math.max(0, self.scroll_offset - self.step)
		self:reshaped()
		self:changed(self.scroll_offset)
	elseif dir == "down" then
		self.scroll_offset = math.min(self.scroll_offset + self.step, self.max - self.page)
		self.scroll_offset = math.max(0, self.scroll_offset)
		self:reshaped()
		self:changed(self.scroll_offset)
	end
end

Scrollbar.set_range = function(self, max, scroll_offset, page)
	if scroll_offset then self.scroll_offset = scroll_offset end
	if page then self.page = page end
	self.max = max
	self.scroll_offset = math.max(0, math.min(self.scroll_offset, self.max - self.page))
	self:reshaped()
end

return Scrollbar


