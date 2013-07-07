local ModifierSpec = require("core/specs/modifier")

-- Increase or decrease health.
local PhysicalDamageModifier = ModifierSpec{
	name = "physical damage",
	categories =
	{
		["harmful"] = true,
		["melee"] = true,
		["physical"] = true
	},
	actions =
	{
		["bite"] = true,
		["dragon bite"] = true,
		["explode"] = true,
		["left claw"] = true,
		["ranged"] = true,
		["right hand"] = true,
		["right claw"] = true,
		["tackle"] = true
	},
	effect = "impact1",
	icon = "missing1", --FIXME
	modifiers =
	{
		["physical damage"] = 1
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
PhysicalDamageModifier.start = function(modifier, value)
	if not modifier.object then return end
	-- Randomize the amount.
	local val = math.max(1, value + value * 0.5 * math.random())
	-- Apply unless friendly fire.
	if not modifier.owner.client or not modifier.object.client then
		modifier.object:damaged{amount = val, point = modifier.point, type = "physical"}
	end
	-- Anger hurt actors.
	if val > 0 then
		modifier.object:add_enemy(modifier.owner)
	end
end