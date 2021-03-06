local UiBinding = require("ui/widgets/binding")

Ui:add_state{
	state = "controls",
	label = "Controls",
	background = function()
		if Client.player_object then return end
		return "mainmenu1"
	end,
	init = function()
		-- Get the control actions.
		local actions = {}
		for k,v in pairs(Client.bindings.dict_name) do
			table.insert(actions, v)
		end
		table.sort(actions, function(a,b) return a.name < b.name end)
		-- Create the binding widgets.
		local widgets = {}
		for k,v in pairs(actions) do
			table.insert(widgets, UiBinding(v))
		end
		return widgets
	end}
