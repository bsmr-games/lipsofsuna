Sound.music_mode = "menu"

--- Switches the music track.
-- @param clss Sound class.
-- @param mode Music mode. ("boss"/"char"/"game"/"menu")
Sound.switch_music_track = function(clss, mode)
	local modes = {
		boss = {"fairytale7", "fairytale9"},
		char = {"fairytale2", "fairytale10"},
		game = {"fairytale1", "fairytale3", "fairytale4", "fairytale5",
		        "fairytale6", "fairytale11", "fairytale12", "fairytale13"},
		menu = {"fairytale8"}};
	if mode then clss.music_mode = mode end
	local tracks = modes[clss.music_mode];
	clss.music_fading = 2.0
	clss.music_looping = (#tracks > 1)
	clss.music_volume = Views.Options.inst.music_volume
	clss.music = tracks[math.random(1, #tracks)]
end

Eventhandler{type = "music-ended", func = function(self, args)
	Sound:switch_music_track()
end}
