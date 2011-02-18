local oldrequire = require
require = function(arg)
	local s,e = string.find(arg, "core/")
	if s then
		local name = string.sub(arg, e + 1)
		if not Program:load_extension(name) then
			error("loading extension `" .. name .. "' failed")
		end
	else
		oldrequire(arg)
	end
end
string.split = function(self, sep)
	local sep,fields = sep or " ", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

-- Handle command line arguments.
require "common/settings"
if not Settings:parse_command_line() then
	print(Settings:usage())
	return
end

require "core/database"
require "core/network"
require "core/tiles"
require "core/tiles-physics"
Voxel.blocks_per_line = 4
Voxel.tiles_per_line = 24
require "common/bitwise"
require "common/color"
require "common/crafting"
require "common/effect"
require "common/eventhandler"
require "common/faction"
require "common/feat"
require "common/iconspec"
require "common/itemspec"
require "common/material"
require "common/marker"
require "common/model"
require "common/names"
require "common/object"
require "common/obstaclespec"
require "common/pattern"
require "common/protocol"
require "common/quest"
require "common/regionspec"
require "common/sectors"
require "common/species"
require "common/thread"
require "common/timer"

if Settings.server then
require "core/object-physics"
require "common/vision"
require "common/inventory"
require "common/skills"
Object.load_meshes = false
else
require "core/animation"
require "core/graphics"
require "core/camera"
require "core/object-render"
require "core/render"
require "core/sound"
require "core/speech"
require "core/tiles-physics"
require "core/tiles-render"
require "core/reload"
require "core/widgets"
require "client/shader"
require "client/shaders/skeletal"
end

require "content/effects"
require "content/factions"
require "content/feats"
require "content/icons"
require "content/items"
require "content/materials"
require "content/obstacles"
require "content/patterns"
require "content/regions"
require "content/species"
require "content/quests/charasillusion"
require "content/quests/erinyes"
require "content/quests/mourningadventurer"
require "content/quests/peculiarpet"
require "content/quests/portaloflips"
require "content/quests/portalofmidguard"
require "content/quests/rootsofworld"
require "content/quests/sanctuary"

if Settings.quit then Program.quit = true end
if Settings.server then
	require "server/main"
else
	require "client/main"
end
