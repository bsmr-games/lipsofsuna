Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "adamantium sword",
	effect = "craftmetal1",
	materials = {["adamantium"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium sword",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default two-handed equipped"},
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	icon = "sword3",
	modifiers = {["physical damage"] = 20},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "sword6"}

CraftingRecipeSpec{
	name = "aquanite sword",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite sword",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default sword equipped"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 500,
	icon = "sword2",
	modifiers = {["cold damage"] = 8, ["physical damage"] = 8},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "sword5"}

CraftingRecipeSpec{
	name = "crimson sword",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson sword",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default sword equipped"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 500,
	icon = "sword2",
	modifiers = {["fire damage"] = 8, ["physical damage"] = 8},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "sword5"}

CraftingRecipeSpec{
	name = "iron sword",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron sword",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default sword equipped"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 220,
	icon = "sword2",
	modifiers = {["physical damage"] = 5},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "sword1"}

CraftingRecipeSpec{
	name = "stone sword",
	effect = "craftmetal1",
	materials = {["granite stone"] = 5},
	modes = {["metal"] = true, ["default"] = true}}

Itemspec{
	name = "stone sword",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default sword equipped"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 130,
	icon = "sword1",
	modifiers = {["physical damage"] = 5},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "sword1"}

CraftingRecipeSpec{
	name = "wooden sword",
	effect = "craftwood1",
	materials = {["log"] = 5},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "wooden sword",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default sword equipped"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 100,
	icon = "sword1",
	modifiers = {["physical damage"] = 5},
	modifiers_bonus = {"melee"},
	mass = 7,
	model = "woodsword",
	water_gravity = Vector(0,6,0)}
