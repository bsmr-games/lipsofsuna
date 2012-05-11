Operators.world = Class()

--- Gets the currently targeted object.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @return Object, or nil.
Operators.world.get_target_object = function(self)
	local object = Target.target_object
	if not object then return end
	if not object.realized then return end
	return object
end

--- Gets the usage actions of the currently targeted object.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @return List of actions, or nil.
Operators.world.get_target_usages = function(self)
	-- Get the targeted object.
	local object = self:get_target_object()
	if not object then return end
	-- Get the object spec.
	local spec = object.spec
	if not spec then return end
	local actions = {}
	-- Add the spec actions.
	if spec.get_use_actions then
		for k,v in pairs(spec:get_use_actions()) do
			table.insert(actions, {k, v.label or v.name, v})
		end
	end
	-- Add the hard-coded actions.
	if spec.type == "actor" then
		if spec.dialog then
			if object.dead then
				table.insert(actions, {"dialog", "Evaluate", nil})
			else
				table.insert(actions, {"dialog", "Chat", nil})
			end
		end
		if object.dead then
			table.insert(actions, {"loot", "Loot", nil})
		else
			table.insert(actions, {"pickpocket", "Pickpocket", nil})
		end
	elseif spec.type == "item" then
		table.insert(actions, {"pick up", "Pick up", nil})
	end
	-- Return the list if it's not empty.
	if not actions[1] then return end
	return actions
end

--- Triggers the use action of the object.
--
-- Context: Any.
--
-- @param self Operator.
-- @param object Object.
-- @param action Action spec.
Operators.world.use_object = function(self, object, action)
	-- Open subscribed inventories directly.
	if action.name == "loot" and Operators.inventory:get_inventory_by_id(object.id) then
		Client.data.inventory.id = object.id
		Ui.state = "loot"
		return
	end
	-- Send a normal use command otherwise.
	Network:send{packet = Packet(packets.PLAYER_USE_WORLD,
		"uint32", object.id,
		"string", action.name)}
	-- Open the dialog screen if appropriate.
	if action.dialog then
		Client.active_dialog_object = object
		Ui.state = "dialog"
	else
		Ui:pop_state()
	end
end