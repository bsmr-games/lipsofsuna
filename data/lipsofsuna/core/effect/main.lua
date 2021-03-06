local Client = require("core/client/client")
local EffectManager = require("core/effect/effect-manager")
local Eventhandler = require("system/eventhandler")
local MusicManager = require("core/effect/music-manager")

Main.main_start_hooks:register(110, function()
	Main.effect_manager = EffectManager()
	Main.music_manager = MusicManager()
end)

Client:register_speech_hook(0, function(args)
	Main.effect_manager:speech(args)
end)

Client:register_update_hook(40, function(secs)
	-- Update effects.
	-- Must be done after objects to ensure correct anchoring.
	Main.timing:start_action("effect")
	Main.effect_manager:update(secs)
	Main.music_manager:update(secs)
end)

Main.game_end_hooks:register(40, function()
	if Main.effect_manager then
		Main.effect_manager:detach_scene_nodes()
	end
end)

Eventhandler{type = "music-ended", func = function(self, args)
	Main.music_manager:cycle_track(true)
end}
