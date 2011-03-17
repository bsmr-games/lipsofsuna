Views.Inventory = Class(Widget)
Views.Inventory.mode = "inventory"

--- Creates a new inventory view.
-- @param clss Inventory view class.
-- @return Inventory view.
Views.Inventory.new = function(clss)
	local self = Widget.new(clss, {cols = 3, rows = 1, spacings = {0, 0}})
	self:set_request{width = 100}
	self.title = Widgets.Frame{style = "title", text = "Inventory"}
	self.group = Widget{cols = 1, rows = 3, spacings = {0, 0}}
	self.group:set_child{col = 1, row = 2, widget = self.title}
	self.group:set_expand{col = 1, row = 1}
	self.containers = Widget{rows = 3, spacings = {0, 0}}
	self.containers:set_expand{row = 1}
	self:set_child{col = 1, row = 1, widget = self.group}
	self:set_child{col = 2, row = 1, widget = self.containers}
	-- Weight widget.
	-- This is injected to the owned inventory widget.
	self.group_weight = Widget{rows = 1, cols = 4, spacings = {0,0}, margins={5,0,0,0}}
	self.label_weight1 = Widgets.Label{text = "Weight:"}
	self.label_weight2 = Widgets.Label()
	self.label_weight3 = Widgets.Label{text = "/"}
	self.label_weight4 = Widgets.Label()
	self.group_weight:set_child{col = 1, row = 1, widget = self.label_weight1}
	self.group_weight:set_child{col = 2, row = 1, widget = self.label_weight2}
	self.group_weight:set_child{col = 3, row = 1, widget = self.label_weight3}
	self.group_weight:set_child{col = 4, row = 1, widget = self.label_weight4}
	return self
end

--- Adds a container to the inventory.
-- @param self Inventory view.
-- @param widget Container widget.
-- @param type Container type number.
-- @param own True if this is the main inventory.
Views.Inventory.add_container = function(self, widget, own)
	if own then
		widget.button_close.visible = false
		self.container = widget
		self.group:set_child{col = 1, row = 3, widget = widget}
		widget:set_extra_widget(self.group_weight)
	else
		local text = widget.crafting and "Workbench" or "Loot"
		widget.button_close.visible = true
		widget.closed = function(w) self:remove_container(w) end
		self.containers:append_col(Widget(), Widgets.Frame{style = "title", text = text}, widget)
		Gui:set_mode("inventory")
	end
end

--- Removes a container from the inventory view.
-- @param self Inventory view.
-- @param widget Container widget.
Views.Inventory.remove_container = function(self, widget)
	-- Remove main inventory.
	if self.container == widget then
		self.container = nil
		self:set_child{col = 1, row = 2}
	end
	-- Remove looted containers.
	for i = 1,self.containers.cols do
		local w = self.containers:get_child{col = i, row = 3}
		if w == widget then
			self.containers:remove{col = i}
			break
		end
	end
end

Views.Inventory.back = function(self)
	Gui:set_mode("menu")
end

--- Closes the inventory view.
-- @param self Inventory view.
Views.Inventory.close = function(self)
	-- Close containers.
	for i = 1,self.containers.cols do
		local w = self.containers:get_child{col = i, row = 3}
		if w then w:close() end
	end
end

--- Enters the inventory view.
-- @param self Inventory view.
Views.Inventory.enter = function(self)
end

--- Gets the contents of an inventory or an equipment slot.
-- @param self Inventory view.
-- @param args Arguments.<ul>
--   <li>id: Container ID.</li></ul>
--   <li>slot: Slot name.</li></ul>
-- @return Item widget or nil.
Views.Inventory.get_item = function(self, args)
	local cont = Widgets.Container:find(args.id or self.id)
	if not cont then return end
	return cont:get_item(args)
end

--- Sets the contents of an equipment slot.
-- @param self Inventory view.
-- @param slot Slot name.
-- @param name Item name.
-- @param count Item count.
Views.Inventory.set_item = function(self, slot, name, count)
	local widget = self.equipment.dict_name[slot]
	if widget then
		local spec = Itemspec:find{name = name}
		widget.text = name or ""
		widget.icon = spec and spec.icon
		widget.count = count or 1
	end
end

--- Sets the displayed weight and burdening limit.
-- @param self Inventory view.
-- @param weight Weight in kilograms.
-- @param limit Burdening limit in kilograms.
Views.Inventory.set_weight = function(self, weight, limit)
	self.label_weight2.text = tostring(weight)
	self.label_weight4.text = tostring(limit)
	if weight <= limit then
		self.label_weight2.color = {1,1,1,1}
	else
		self.label_weight2.color = {1,0,0,1}
	end
end

------------------------------------------------------------------------------

Views.Inventory.inst = Views.Inventory()

-- Loots a container.
Protocol:add_handler{type = "INVENTORY_CREATED", func = function(event)
	local ok,id,typ,nam,size,own = event.packet:read("uint32", "string", "string", "uint8", "bool")
	if not ok then return end
	local mapping = {
		item = function() return Itemspec:find{name = nam} end,
		obstacle = function() return Obstaclespec:find{name = nam} end,
		species = function() return Species:find{name = nam} end}
	local func = mapping[typ]
	local spec = func and func()
	local cont = Widgets.Container{id = id, size = size, spec = spec}
	Views.Inventory.inst:add_container(cont, own)
end}

-- Closes a container.
Protocol:add_handler{type = "INVENTORY_CLOSED", func = function(event)
	local ok,id = event.packet:read("uint32")
	if not ok then return end
	-- Close the container widget if it's still open.
	local cont = Widgets.Container:find(id)
	if cont then cont:close(true) end
	-- Clear interrupted item drags.
	-- The container may close if another player picks it up or moves
	-- it out of reach. If that happens, any drags from that container
	-- need to be cancelled to avoid problems.
	if Drag.drag and Drag.drag[2] == id then
		Drag:cancel()
	end
end}

-- Adds an item to a container.
Protocol:add_handler{type = "INVENTORY_ITEM_ADDED", func = function(event)
	local ok,id,islot,sslot,count,name = event.packet:read("uint32", "uint8", "string", "uint32", "string")
	if not ok then return end
	local cont = Widgets.Container:find(id)
	if not cont then return end
	local spec = Itemspec:find{name = name}
	local icon = spec and spec.icon
	cont:set_item{slot = (sslot ~= "" and sslot or islot), icon = icon, name = name, count = count}
	-- Clear interrupted item drags.
	-- When multiple players try to drag the same item, the fastest drag
	-- succeeds. Other drags need to be cancelled so that the drag icon
	-- disappears and the source slot isn't incorrectly left grayed out.
	if Drag.drag and Drag.drag[2] == id and Drag.drag[3] == slot then
		Drag:cancel()
	end
end}

-- Removes an item from a container.
Protocol:add_handler{type = "INVENTORY_ITEM_REMOVED", func = function(event)
	local ok,id,islot,sslot = event.packet:read("uint32", "uint8", "string")
	if not ok then return end
	local cont = Widgets.Container:find(id)
	if not cont then return end
	-- Remove from the container.
	local slot = (sslot ~= "" and sslot or islot)
	cont:set_item{slot = slot}
	-- Clear interrupted item drags.
	-- When multiple players try to drag the same item, the fastest drag
	-- succeeds. Other drags need to be cancelled so that the drag icon
	-- disappears and the source slot isn't incorrectly left grayed out.
	if Drag.drag and Drag.drag[2] == id and Drag.drag[3] == slot then
		Drag:cancel()
	end
end}

-- Updates the weight and burdening limit of the player.
Protocol:add_handler{type = "PLAYER_WEIGHT", func = function(event)
	local ok,w,l = event.packet:read("uint16", "uint16")
	if not ok then return end
	Views.Inventory.inst:set_weight(w, l)
end}
