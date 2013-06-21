local Class = require("system/class")
local Feat = require("arena/feat")

local Quickpage = Class("Quickpage")

--- Creates a new quickslots page.
-- @param clss Quickpage class.
-- @param args Arguments.<ul>
--   <li>type: Page type.</li></ul>
-- @return Quickpage.
Quickpage.new = function(clss, args)
	-- Create self.
	local self = Class.new(clss)
	self.type = args and args.type or "feats"
	-- Create the slots.
	self.buttons = {}
	for i = 1,10 do self.buttons[i] = {} end
	-- Load feats from the database.
	self:load()
	return self
end

--- Clears a quickslot assignment.
-- @param self Quickpage.
-- @param index Quickslot index number.
Quickpage.assign_none = function(self, index)
	if index > #self.buttons then return end
	self.buttons[index].feat = nil
	self.buttons[index].item = nil
	self:save()
end

--- Assigns a feat to a quickslot.
-- @param self Quickpage.
-- @param index Quickslot index number.
-- @param feat Feat to assign.
Quickpage.assign_feat = function(self, index, feat)
	if not feat then return end
	if index > #self.buttons then return end
	self.buttons[index].feat = feat
	self.buttons[index].item = nil
	self:save()
end

--- Assigns an item to a quickslot.
-- @param self Quickpage class.
-- @param index Quickslot index number.
-- @param item Item to assign.
Quickpage.assign_item = function(self, index, item)
	if index > #self.buttons then return end
	self.buttons[index].feat = nil
	self.buttons[index].item = item.name
	self:save()
end

--- Called when the quickslot is clicked or accessed by a hotkey.<br/>
-- If a quickslot is activated while dragging an item, the item is assigned
-- to the slot. Otherwise, the item or feat in the slot is activated.
-- @param self Quickpage class.
-- @param index Quickslot index number.
Quickpage.activate = function(self, index)
	local feat = self.buttons[index].feat
	local item = self.buttons[index].item
	if feat then
		-- Activate a feat.
		local names = {}
		local values = {}
		for i = 1,3 do
			names[i] = feat.effects[i] and feat.effects[i][1] or ""
			values[i] = feat.effects[i] and feat.effects[i][2] or 0
		end
		Main.messaging:client_event("cast spell", feat.animation or "",
			names[1], values[1], names[2], values[2],
			names[3], values[3], true)
		Main.messaging:client_event("cast spell", feat.animation or "",
			names[1], values[1], names[2], values[2],
			names[3], values[3], false)
	elseif item then
		-- Use an item.
		local cont = Client.player_object.inventory
		if not cont then return end
		local index = cont:get_index_by_name(item)
		if not index then return end
		--Commands:use(cont.id, slot)
	end
end

--- Loads feats from the database.
-- @param self Quickpage.
Quickpage.load = function(self)
	-- Only load feats.
	if self.type ~= "feats" then return end
	--self.db = Client.db
	--self.db:query("CREATE TABLE IF NOT EXISTS quickslots (slot INTEGER PRIMARY KEY,item TEXT,feat TEXT);")
	self.loading = true
	-- First set defaults.
	self:assign_feat(1, Feat("spell on self", {{"restore health", 10}}))
	self:assign_feat(2, Feat("ranged spell", {{"fire damage", 10}}))
	self:assign_feat(3, Feat("spell on self", {{"light", 1}}))
	-- Load from the database.
	--local rows = self.db:query("SELECT slot,item,feat FROM quickslots;")
	local rows = {}
	for k,v in ipairs(rows) do
		if v[1] >= 1 and v[1] <= #self.buttons then
			if v[2] and v[2] ~= "" then
				self:assign_item(v[1], v[2])
			elseif v[3] and v[3] ~= "" then
				self:assign_feat(v[1], Feat:load{data = v[3]})
			else
				self:assign_none(v[1])
			end
		end
	end
	self.loading = nil
end

--- Saves feats to the database.
-- @param self Quickpage.
Quickpage.save = function(self)
	if self.type ~= "feats" then return end
	if self.loading then return end
	for k,v in ipairs(self.buttons) do
		local feat = v.feat and v.feat:write()
		local item = v.item
		--self.db:query("REPLACE INTO quickslots (slot,item,feat) VALUES (?,?,?);", {k, item, feat})
	end
end

return Quickpage
