local Graphics = require("system/graphics")
local Hudcompass = require("ui/hud/compass")
local HudCompanion = require("ui/hud/companion")
local Hudfps = require("ui/hud/fps")
local Hudlog = require("ui/hud/log")
local Hudmodifiers = require("ui/hud/modifiers")
local Hudnotification = require("ui/hud/notification")
local Hudtarget = require("ui/hud/target")
local UiIcon = require("ui/widgets/icon")
local UiStat = require("ui/widgets/stat")

Ui:add_hud{
	id = "health",
	active = function() return Ui.root == "play" end,
	init = function() return UiStat("health", 2) end}

Ui:add_hud{
	id = "willpower",
	active = function() return Ui.root == "play" end,
	init = function() return UiStat("willpower", 1) end}

Ui:add_hud{
	id = "chat",
	active = function() return Ui.root == "play" end,
	init = function() return Hudlog() end}

Ui:add_hud{
	id = "crosshair",
	active = function() return Ui.root == "play" end,
	init = function()
		local self = UiIcon(Main.specs:find_by_name("IconSpec", "crosshair1"))
		self.update = function(self, secs)
			local mode = Graphics:get_video_mode()
			local padx = mode[1] - self.icon.size[1]
			local pady = mode[2] - self.icon.size[2]
			self:set_offset(Vector(padx / 2, pady / 2))
		end
		return self
	end}

Main.main_start_hooks:register(1002, function(secs)
	Main.windows:add_window(Hudcompass())
	Main.windows:add_window(Hudfps())
	Main.windows:add_window(HudCompanion())
end)

Ui:add_hud{
	id = "modifier",
	active = function() return Ui.root == "play" end,
	init = function() return Hudmodifiers() end}

Ui:add_hud{
	id = "notification",
	active = function() return Ui.root == "play" end,
	init = function() return Hudnotification() end}

Ui:add_hud{
	id = "target",
	active = function() return Ui:get_state() == "play" end,
	init = function() return Hudtarget() end}
