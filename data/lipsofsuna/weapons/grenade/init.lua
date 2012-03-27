Program:add_path(Mod.path)

Itemspec{
	name = "aquanite grenade",
	model = "grenade1",
	icon = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_charge = "charge grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	damage_mining = 1,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 1,
	influences_base = {cold = -15, physical = -5},
	influences_bonus = {"explosives"},
	mass = 5,
	stacking = true}

Itemspec{
	name = "crimson grenade",
	model = "grenade1",
	icon = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_charge = "charge grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	effect_craft = "craftmetal1",
	influences_base = {fire = -15, physical = -5},
	influences_bonus = {"explosives"},
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	mass = 5,
	stacking = true}
