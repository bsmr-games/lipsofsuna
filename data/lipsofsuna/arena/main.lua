local Arena = require("arena/arena")
local ArenaCamera = require("arena/camera")
local Client = require("core/client/client")
local CombatUtils = require("arena/combat-utils")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("arena", ArenaCamera())
end)

Client:register_start_hook(0, function()
	if Main.settings.arena then
		Main.arena = Arena()
		Ui:set_state("arena")
		return Hooks.STOP
	end
end)

Main.main_start_hooks:register(0, function(secs)
	Main.combat_utils = CombatUtils()
end)

Main.update_hooks:register(0, function(secs)
	if Main.arena then
		Main.timing:start_action("arena")
		Main.arena:update()
	end
end)
