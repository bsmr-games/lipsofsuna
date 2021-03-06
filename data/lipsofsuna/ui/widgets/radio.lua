local Class = require("system/class")
local UiToggle = require("ui/widgets/toggle")

local UiRadio = Class("Uiradio", UiToggle)

UiRadio.new = function(clss, label, group, changed)
	local self = UiToggle.new(clss, label, changed)
	self.radio_group = group
	self.hint = "$A: Select\n$$B\n$$U\n$$D"
	return self
end

UiRadio.apply = function(self)
	-- Reactivation does nothing.
	if self.value then return end
	-- Deactivate other toggles in the group.
	for k,v in Ui.widgets:get_children() do
		if v.radio_group == self.radio_group then
			if v ~= self and v.value then
				v.value = false
				v.need_repaint = true
			end
		end
	end
	-- Activate this radio toggle.
	self.value = true
	self.need_repaint = true
	self:changed()
	Main.effect_manager:play_global("uitoggle1")
end

UiRadio.changed = function(self)
end

return UiRadio
