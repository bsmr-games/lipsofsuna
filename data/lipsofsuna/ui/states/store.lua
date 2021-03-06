local UiButton = require("ui/widgets/button")
local UiInvStore = require("ui/widgets/invstore")

Ui:add_state{
	state = "store",
	history = "inventory",
	label = "Store"}

Ui:add_widget{
	state = "store",
	widget = function()
		local object = Client.player_object
		if not object then return end
		local widgets = {}
		for index = 1,object.inventory.size do
			local item = object.inventory:get_object_by_index(index)
			local slot = object.inventory:get_slot_by_index(index)
			local data = item and {
				text = item.spec.name,
				count = item:get_count(),
				icon = item.spec.icon}
			table.insert(widgets, UiInvStore(Client.data.inventory.id, data, index, slot))
		end
		return widgets
	end}

Ui:add_widget{
	state = "store",
	widget = function()
		local w = UiButton("Loot", function() Ui:pop_state() end)
		w.hint = "$A: Enter\n$$B\n$$U\n$$D"
		return w
	end}
