local TerrainManager = require("core/terrain/terrain-manager")

Main.game_start_hooks:register(0, function()
	Main.game.terrain = TerrainManager(8, 0.75,
		Main.game.database, Main.game.enable_unloading,
		Main.game.enable_generation, Main.game.enable_graphics)
end)

Main.update_hooks:register(0, function(secs)
	Main.timing:start_action("terrain")
	if Main.game then
		Main.game.terrain:update(tick)
	end
end)