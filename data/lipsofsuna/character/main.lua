local Client = require("core/client/client")
local Chargen = require("character/chargen")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Character", function()
	-- Configure messaging.
	Main.messaging:set_transmit_mode(true, true)
	-- FIXME: Initialize the game.
	Main.game = Game("benchmark")
	Main.game:start()
	Main.game_create_hooks:call()
	Main.objects:set_unloading()
	-- Start the subsystems.
	Client.chargen:init(true)
	Ui:set_state("chargen")
end)

Main.game_end_hooks:register(0, function(secs)
	Client.chargen:reset()
end)

Main.main_start_hooks:register(130, function()
	Client.chargen = Chargen()
	Client.camera_manager:register_camera("chargen", Client.chargen.camera)
end)

Client:register_reset_hook(10, function()
	Client.chargen:reset()
end)
