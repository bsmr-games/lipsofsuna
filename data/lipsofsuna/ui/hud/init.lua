if not Settings then return end
if Settings.server then return end

require(Mod.path .. "modifiers")

Ui:add_hud{
	id = "health",
	init = function() return Widgets.Uistat("health", 2) end}

Ui:add_hud{
	id = "willpower",
	init = function() return Widgets.Uistat("willpower", 1) end}

Ui:add_hud{
	id = "chat",
	init = function()
		local self = Widgets.Log()
		self.update = function(self, secs)
			local mode = Program.video_mode
			self.offset = Vector(5, mode[2] - 200)
		end
		return self
	end}

-- TODO
--[[Ui:add_hud{
	id = "compass",
	init = function()
		local self = Widgets.Compass()
		self.timer = 0
		self.update = function(self, secs)
			self.timer = self.timer + secs
			if self.timer < 0.1 then return end
			self.timer = 0
			self.look_direction = math.pi - clss.rotation_curr.euler[1]
			self.quest_direction = Client.views.quests:get_compass_direction()
			self.quest_distance = Client.views.quests:get_compass_distance()
			self.quest_height = Client.views.quests:get_compass_height()
		end
		return self
	end}]]

Ui:add_hud{
	id = "crosshair",
	init = function()
		local self = Widgets.Icon{icon = Iconspec:find{name = "crosshair1"}}
		self.update = function(self, secs)
			local mode = Program.video_mode
			local padx = mode[1] - self.icon.size[1]
			local pady = mode[2] - self.icon.size[2]
			self.offset = Vector(padx / 2, pady / 2)
		end
		return self
	end}

Ui:add_hud{
	id = "fps",
	init = function()
		local self = Widgets.Label{valign = 1, request = Vector(60,20)}
		self.update = function(self, secs)
			local mode = Program.video_mode
			self.offset = Vector(mode[1] - 60, mode[2] - 20)
			self.text = "FPS: " .. tostring(math.floor(Program.fps + 0.5))
		end
		return self
	end}

Ui:add_hud{
	id = "modifier",
	init = function()
		return Widgets.Hudmodifiers()
	end}

Ui:add_hud{
	id = "notification",
	init = function()
		return Widgets.Notification()
	end}