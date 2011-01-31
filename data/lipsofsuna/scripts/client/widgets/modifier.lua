Widgets.Modifier = Class(Widget)
Widgets.Modifier.DEFAULT = {image="icons1", offset={0,222}, size={34,34}}

Widgets.Modifier.setter = function(self, key, value)
	if key == "icon" then
		if self.icon ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "time" then
		Widget.setter(self, key, value)
		local t = math.ceil((value + 0.1) / 5) * 5
		if t ~= self.time_coarse then
			self.time_coarse = t
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.Modifier.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.icon = self.icon or clss.DEFAULT
	self.font = self.font or "bigger"
	return self
end

Widgets.Modifier.reshaped = function(self)
	local icon = self.icon or self.DEFAULT
	self:set_request{
		internal = true,
		width = 34,
		height = 44}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = icon.size,
		source_image = icon.image,
		source_position = icon.offset,
		source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	if self.time_coarse < 10000 then
		self:canvas_text{
			dest_position = {2,2},
			dest_size = {w,h},
			text = tostring(self.time_coarse),
			text_alignment = {0.5,1},
			text_color = {0,0,0,1},
			text_font = self.font}
		self:canvas_text{
			dest_position = {0.5,0},
			dest_size = {w,h},
			text = tostring(self.time_coarse),
			text_alignment = {0.5,1},
			text_color = {1,1,1,1},
			text_font = self.font}
	end
	self:canvas_compile()
end

Widgets.Modifier.pressed = function(self, args)
end
