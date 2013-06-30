local Class = require("system/class")
local Noise = require("system/noise")
local Sector = require("system/sector")

Generator.sector_types.Main = Class("Generator.Main")

Generator.sector_types.Main.init = function(self)
	self.scale1 = Vector(1,1,1) * (0.01 * Voxel.tile_scale)
end

--- Generates a sector of a suitable type.
-- @param self Main generator.
-- @param sector Sector index.
-- @param yield Yield function.
Generator.sector_types.Main.generate = function(self, sector, yield)
	local w = Voxel.tiles_per_line
	local size = Vector(w,w,w)
	local pos = Sector:get_tile_by_id(sector)
	yield()
	local t = self:get_sector(pos, size)
	local g = Generator.sector_types[t]
	yield()
	if not g then
		Voxel:fill_region{point = pos, size = size, tile = 0}
	else
		g:generate(pos, size, yield)
	end
end

Generator.sector_types.Main.get_cluster = function(self, pos, size)
	local s = Voxel.tiles_per_line
	local a = {}
	for x = -1,1 do
		a[x] = {}
		for y = -1,1 do
			a[x][y] = {}
			for z = -1,1 do
				a[x][y][z] = self:get_sector(pos + Vector(x*s,y*s,z*s), size)
			end
		end
	end
	return a
end

Generator.sector_types.Main.get_sector = function(self, pos, size)
	-- FIXME: Replace with stick terrain generation.
	do return "Empty" end
	if pos.y > 1200 and pos.y < 1300 then return "Overworld" end
	if pos.y > 1100 then return "Empty" end
	-- Predefined sectors.
	-- The map generator allocates some sectors for towns. They're formatted
	-- using a specific town generator.
	local s = Server.generator.sectors[Sector:get_id_by_tile(pos)]
	if s then return s end
	if not Server.generator:validate_rect(pos, size) then return "Town" end
	-- Map boundaries.
	-- The map is surrounded by special border sectors to prevent things from
	-- falling outside of the map.
	local min = 10 * Voxel.tiles_per_line
	local max = 100 * Voxel.tiles_per_line
	if pos.x < min or pos.y < min or pos.z < min or pos.x > max or pos.y > max or pos.z > max then return "Border" end
	-- Random ruins.
	-- Ruins can appear anywhere so their generation is a bit special. They're
	-- generated if the random number falls inside one of a few gaps in the range.
	local n1 = Noise:perlin_noise(pos, self.scale1, 1, 3, 0.5, Server.generator.seed1)
	local n2 = math.abs(3 * n1)
	if math.ceil(n2) - n2 < 0.2 then return "Ruins" end
	-- Random sectors.
	-- Other sector types are allocated a specific range inside which they appear.
	-- TODO: The range should depend on the depth so that magma and water are more common in specific map layers.
	if n1 < -0.9 then return "Volcano" end
	if n1 > 0.8 then return "Ocean" end
	if n1 > 0.2 then return "Forest" end
	return "Dungeon"
end