string.split = function(self, sep)
	local sep,fields = sep or " ", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

require "system/config-file"
require "system/core"
require "system/database"
require "system/eventhandler"
require "system/network"
require "system/thread"
require "system/math"
require "system/noise"
require "system/object"
require "system/object-physics"
require "system/physics"
require "system/tiles"
require "system/tiles-physics"
require "system/tiles-physics"
require "system/timer"

-- Handle command line arguments.
require "common/settings"
if not Settings:parse_command_line() then
	print(Settings:usage())
	return
end

Voxel.blocks_per_line = 4
Voxel.tiles_per_line = 24
require "common/bitwise"
require "common/color"
require "common/crafting"
require "common/dialogspec"
require "common/effect"
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
require "common/serialize"
require "common/sectors"
require "common/species"
require "common/voxel"

if Settings.server then
require "common/vision"
require "common/inventory"
require "common/skills"
Object.load_meshes = false
Physics.enable_simulation = true
else
require "system/animation"
require "system/graphics"
require "system/camera"
require "system/keysym"
require "system/object-render"
require "system/render"
require "system/sound"
require "system/speech"
require "system/tiles-render"
require "system/reload"
require "system/widgets"
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
require "content/dialogs/celine"
require "content/dialogs/chara"
require "content/dialogs/charasportal"
require "content/dialogs/elise"
require "content/dialogs/erinyes"
require "content/dialogs/lipscitizen"
require "content/dialogs/noemi"
require "content/dialogs/portalofillusion"
require "content/dialogs/portaloflips"
require "content/dialogs/portalofmidguard"
require "content/dialogs/puppy"
require "content/dialogs/sanctuary"
require "content/quests/charasillusion"
require "content/quests/elisessorrow"
require "content/quests/erinyes"
require "content/quests/peculiarpet"
require "content/quests/portaloflips"
require "content/quests/portalofmidguard"
require "content/quests/rootsofworld"
require "content/quests/sanctuary"

if Settings.quit then Program.quit = true end
if Settings.server then
	require "server/main"
elseif Settings.editor then
	require "editor/main"
else
	require "client/main"
end
