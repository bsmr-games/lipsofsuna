require "client/quickslots"

Views.Feats = Class(Widget)
Views.Feats.mode = "chargen"

--- Creates a new feat editor.
-- @param clss Feats class.
-- @return Feats.
Views.Feats.new = function(clss)
	local self = Widget.new(clss, {cols = 1})
	-- Animation selector.
	local label = Widgets.Label{text = "Type"}
	label:set_request{width = 46}
	self.combo_anim = Widgets.ComboBox()
	self.group_anim = Widget{cols = 2, rows = 1}
	self.group_anim:set_expand{col = 2}
	self.group_anim:set_child{col = 1, row = 1, widget = label}
	self.group_anim:set_child{col = 2, row = 1, widget = self.combo_anim}
	-- Effect selectors.
	self.combo_effect = {}
	self.scroll_effect = {}
	for i = 1,3 do
		self.combo_effect[i] = Widgets.ComboBox()
		self.combo_effect[i]:set_request{width = 100}
		self.scroll_effect[i] = Widgets.Progress{min = 0, max = 100, value = 0, pressed = function(w)
			w.value = w:get_value_at(Client.cursor_pos)
			self:changed()
		end}
		self.scroll_effect[i]:set_request{width = 100}
	end
	self.group_effect = Widget{cols = 3, rows = 3}
	self.group_effect:set_expand{col = 3}
	self.group_effect:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Effect 1"}}
	self.group_effect:set_child{col = 1, row = 2, widget = Widgets.Label{text = "Effect 2"}}
	self.group_effect:set_child{col = 1, row = 3, widget = Widgets.Label{text = "Effect 3"}}
	self.group_effect:set_child{col = 2, row = 1, widget = self.combo_effect[1]}
	self.group_effect:set_child{col = 2, row = 2, widget = self.combo_effect[2]}
	self.group_effect:set_child{col = 2, row = 3, widget = self.combo_effect[3]}
	self.group_effect:set_child{col = 3, row = 1, widget = self.scroll_effect[1]}
	self.group_effect:set_child{col = 3, row = 2, widget = self.scroll_effect[2]}
	self.group_effect:set_child{col = 3, row = 3, widget = self.scroll_effect[3]}
	-- Requirement display.
	self.label_skills = Widgets.Label{valign = 0}
	self.label_reagents = Widgets.Label{valign = 0}
	self.group_req = Widget{rows = 2, cols = 2}
	self.group_req:set_request{height = 100}
	self.group_req:set_expand{col = 1, row = 2}
	self.group_req:set_expand{col = 2}
	self.group_req:set_child{row = 1, col = 1, widget = Widgets.Label{font = "medium", text = "Skills"}}
	self.group_req:set_child{row = 1, col = 2, widget = Widgets.Label{font = "medium", text = "Reagents"}}
	self.group_req:set_child{row = 2, col = 1, widget = self.label_skills}
	self.group_req:set_child{row = 2, col = 2, widget = self.label_reagents}
	-- Quickslot selector.
	self.group_quick = Widget{cols = 12, rows = 1}
	for i = 1,12 do
		local button = Widgets.Button{text = tostring(i), pressed = function() self:show(i) end}
		self.group_quick:set_child{col = i, row = 1, widget = button}
		self.group_quick:set_expand{col = i}
	end
	-- Packing for the dialog.
	self:append_row(Widgets.Label{font = "medium", text = "Feat"})
	self:append_row(self.group_quick)
	self:append_row(self.group_anim)
	self:append_row(self.group_effect)
	self:append_row(self.group_req)
	-- Show the first feat.
	self:show(1)
	return self
end

Views.Feats.back = function(self)
	Gui:set_mode("menu")
end

--- Recalculates the skill and reagent requirements of the currently shown feat.
-- @param self Feats.
Views.Feats.changed = function(self)
	if self.protect then return end
	-- Get effects and their magnitudes.
	local effects = {}
	local values = {}
	for i = 1,3 do
		table.insert(effects, self.combo_effect[i].text)
		table.insert(values, self.scroll_effect[i].value)
	end
	-- Calculate skill and reagent requirements.
	local reagents = {}
	local skills = {}
	for index,name in pairs(effects) do
		local effect = Feateffectspec:find{name = name}
		if effect then
			-- Base skill requirements.
			for skill,value in pairs(effect.skill_base) do
				local val = skills[skill] or 0
				skills[skill] = val + value
			end
			-- Magnitude based skill requirements.
			for skill,mult in pairs(effect.skill_mult) do
				local val = skills[skill] or 0
				skills[skill] = val + mult * values[index]
			end
			-- Base reagent requirements.
			for reagent,value in pairs(effect.reagent_base) do
				local val = reagents[reagent] or 0
				reagents[reagent] = val + value
			end
			-- Magnitude based reagent requirements.
			for reagent,mult in pairs(effect.reagent_mult) do
				local val = reagents[reagent] or 0
				reagents[reagent] = val + mult * values[index]
			end
		end
	end
	for k,v in pairs(skills) do
		skills[k] = math.max(1, math.ceil(v))
	end
	for k,v in pairs(reagents) do
		reagents[k] = math.max(1, math.ceil(v))
	end
	-- Display skill requirements.
	local skill_list = {}
	for k,v in pairs(skills) do
		table.insert(skill_list, k .. ": " .. v)
	end
	table.sort(skill_list)
	local skill_str = ""
	for k,v in ipairs(skill_list) do
		skill_str = skill_str .. (skill_str ~= "" and "\n" or "") .. v
	end
	self.label_skills.text = skill_str
	-- Display reagent requirements.
	local reagent_list = {}
	for k,v in pairs(reagents) do
		table.insert(reagent_list, k .. ": " .. v)
	end
	table.sort(reagent_list)
	local reagent_str = ""
	for k,v in ipairs(reagent_list) do
		reagent_str = reagent_str .. (reagent_str ~= "" and "\n" or "") .. v
	end
	self.label_reagents.text = reagent_str
	-- Update the active quickslot.
	local feat = {animation = self.combo_anim.text ~= "" and self.combo_anim.text, effects = {}}
	for i = 1,3 do
		feat.effects[i] = {effects[i], values[i]}
	end
	Quickslots:assign_feat(self.active_slot, feat)
end

--- Sets the race of the character using the feat editor.
-- @param self Feats.
-- @param name Race name.
Views.Feats.set_race = function(self, name)
	local spec = Species:find{name = name}
	if not spec then return end
	-- Rebuild the feat animation list.
	self.dict_anims_id = {{"", nil}}
	for k in pairs(spec.feat_anims) do
		table.insert(self.dict_anims_id, {k, function() self:changed() end})
	end
	table.sort(self.dict_anims_id, function(a, b) return a[1]<b[1] end)
	self.combo_anim:clear()
	for k,v in ipairs(self.dict_anims_id) do
		self.combo_anim:append{text = v[1], pressed = v[2]}
	end
	-- Rebuild the feat effect list.
	self.dict_effects_id = {{"", nil}}
	for k in pairs(spec.feat_effects) do
		table.insert(self.dict_effects_id, {k, function() self:changed() end})
	end
	table.sort(self.dict_effects_id, function(a, b) return a[1]<b[1] end)
	for i = 1,3 do
		self.combo_effect[i]:clear()
		for k,v in ipairs(self.dict_effects_id) do
			self.combo_effect[i]:append{text = v[1], pressed = v[2]}
		end
	end
end

--- Shows the feat for the given quickslot.
-- @param self Feats.
-- @param index Quickslot index.
Views.Feats.show = function(self, index)
	local feat = Quickslots.buttons[index].feat or Feat()
	self.protect = true
	self.combo_anim:activate{text = feat.animation or ""}
	for j = 1,3 do
		local e = feat.effects[j]
		self.combo_effect[j]:activate{text = e and e[1] or ""}
		self.scroll_effect[j].value = e and e[2] or 0
	end
	self.protect = nil
	self.active_slot = index
	self:changed()
end

Views.Feats.inst = Views.Feats()
