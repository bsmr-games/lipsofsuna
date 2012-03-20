Ui:add_state{
	state = "admin/client-stats",
	label = "Client stats"}

Ui:add_widget{
	state = "admin/client-stats",
	widget = function(secs) return Widgets.Uilabel(Client.data.admin.client_stats) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "admin/server-stats",
	label = "Server stats"}

Ui:add_widget{
	state = "admin/server-stats",
	label = "Server stats",
	widget = function(secs) return Widgets.Uilabel(Client.data.admin.server_stats) end}