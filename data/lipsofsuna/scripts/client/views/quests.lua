Views.Quests = Class(Widget)

Views.Quests.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.title = Widgets.Title{text = "Quests",
		back = function() self:back() end,
		close = function() self:close() end,
		help = function() Client.views.help:show("quests") end}
	self.dict_name = {}
	self.sound_play_time = Program.time
	self.list = Widgets.List{page_size = 5}
	self.list.pressed = function(view, row) self:show(row) end
	self.quest_info = Widgets.QuestInfo()
	self.frame = Widgets.Frame{style = "default", cols = 1, rows = 1}
	self.frame:set_child{col = 1, row = 1, widget = self.list}
	self.frame:set_expand{col = 1, row = 1}
	self:set_expand{col = 1, row = 2}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.frame}
	self:set_child{col = 1, row = 3, widget = self.quest_info}
	return self
end

Views.Quests.back = function(self)
	Client:set_mode("menu")
end

Views.Quests.enter = function(self, from, level)
	Gui.menus:open{level = level, widget = self}
end

--- Gets the compass direction for the currently shown quest.<br/>
-- The returned direction is in radians. The coordinate convention is:<ul>
--  <li>north: 0*pi rad</li>
--  <li>east: 0.5*pi rad</li>
--  <li>south: 1*pi rad</li>
--  <li>west: 1.5*pi rad</li></ul>
-- @param self Quest class.
-- @return Compass direction in radians, or nil.
Views.Quests.get_compass_direction = function(self)
	if self.shown_quest then
		local quest = Quest:find{name = self.shown_quest}
		if not quest or not quest.marker then return end
		if not Player.object then return end
		local diff = quest.marker - Player.object.position
		return 0.5 * math.pi + math.atan2(diff.z, -diff.x)
	end
end

--- Gets the distance to the shown quest marker in the X-Z plane.
-- @param self Quest class.
-- @return Compass distance or nil.
Views.Quests.get_compass_distance = function(self)
	if self.shown_quest then
		local quest = Quest:find{name = self.shown_quest}
		if not quest or not quest.marker then return end
		if not Player.object then return end
		local diff = quest.marker - Player.object.position
		diff.y = 0
		return diff.length
	end
end

--- Gets the compass height offset for the currently shown quest.
-- @param self Quest class.
-- @return Compass height offset or nil.
Views.Quests.get_compass_height = function(self)
	if self.shown_quest then
		local quest = Quest:find{name = self.shown_quest}
		if not quest or not quest.marker then return end
		if not Player.object then return end
		local diff = quest.marker - Player.object.position
		return diff.y
	end
end

--- Shows a quest.
-- @param self Quests class.
-- @param name Quest name.
Views.Quests.show = function(self, name)
	local quest = Quest:find{name = name}
	if not quest then return end
	self.quest_info.quest = quest
	self.shown_quest = name
end

--- Updates a quest.
-- @param self Quests class.
-- @param quest Quest.
Views.Quests.update = function(self, quest)
	local button = self.dict_name[quest.name]
	if not button then
		button = Widgets.Button{
			pressed = function(widget) self:show(widget.text) end,
			text = quest.name}
		self.list:append{widget = button}
		self.dict_name[quest.name] = button
	end
	self:show(quest.name)
end
