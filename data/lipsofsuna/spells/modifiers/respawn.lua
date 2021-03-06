local Vector = require("system/math/vector")

local RespawnModifier = Main.specs:find_by_name("ModifierSpec", "respawn")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
RespawnModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
RespawnModifier.update = function(modifier, secs)
	-- Zero the velocity until spawned.
	modifier.object:set_velocity(Vector())
	-- Wait for the sector to load.
	local src = modifier.object:get_position()
	if not Main.terrain:is_point_loaded(src) then
		return true
	end
	-- Find the terrain surface.
	local dst = Vector(src.x, 0, src.z)
	local p = Main.terrain.terrain:cast_ray(src, dst)
	-- Try another position if failed.
	if not p then
		dst.x = src.x + 10 * math.random()
		dst.y = 50000
		dst.z = src.z + 10 * math.random()
		modifier.object:set_position(dst)
		return true
	end
	-- Snap to the terrain surface.
	modifier.object:set_position(p)
end
