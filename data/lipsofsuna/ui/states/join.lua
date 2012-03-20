Ui:add_state{
	state = "join",
	root = "mainmenu",
	label = "Join",
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uitransition("Server list", "join/servers") end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uiconfigoption("join_address") end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uiconfigoption("join_port") end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uiconfigoption("join_account") end}

Ui:add_widget{
	state = "join",
	widget = function()
		return Widgets.Uientry("Password", Client.options.join_password, function(w)
			Client.options.join_password = w.value
		end)
	end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uibutton("Connect", function()
			Settings.address = Client.options.join_address
			Settings.host = false
			Settings.port = Client.options.join_port
			Settings.account = Client.options.join_account
			Settings.password = Client.options.join_password
			Client:join_game()
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "join/servers",
	root = "mainmenu",
	label = "Select server",
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end,
	init = function()
		-- Download the server list.
		Lobby.master = Client.options.master_server
		local servers = Lobby:download_server_list() or {}
		-- Add the local server.
		table.insert(servers, 1, {ip = "localhost", port = 10101,
			name = "Localhost", desc = "Connect to a locally hosted server"})
		-- Create the widgets.
		local widgets = {}
		for k,v in pairs(servers) do
			table.insert(widgets, Widgets.Uiserverinfo(v))
		end
		return widgets
	end}