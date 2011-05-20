------------------------------------------------------------------------------
-- Materials.

-- * materials (label used below in "weapons" section)
-- tile \ state 	label	stone	ore	ingot
-- ==================================================
-- adamantium 		A	4	-	-
-- aquanite 		Q	2	-	-
-- crimson		C	4	-	-
-- granite/ferro/iron	S	5	4	3
-- basalt		S	5	-	-
-- sand chunk/../glass	-	3	-	2
-- soil chunk		-	3	-	-

Itemspec{
	name = "adamantium",
	model = "adamantium1",
	icon = "greymineral1",
	mass = 4,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "aquanite stone",
	model = "aquanitestone1",
	icon = "stone2",
	mass = 2,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "basalt stone",
	model = "basaltstone1",
	icon = "stone1",
	mass = 5,
	categories = {"build", "material"},
	construct_tile = "basalt1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "berry",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "crimson stone",
	model = "crimsonstone1",
	icon = "stone2",
	mass = 4,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "ferrostone",
	model = "ferrostone1",
	icon = "stone2",
	mass = 4,
	categories = {"build", "material"},
	construct_tile = "ferrostone1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "flax",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "fruit",
	model = "fruit1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "glass ingot",
	model = "book-000", -- FIXME
	icon = "ingot1",
	categories = {"material"},
	crafting_materials = {["sand chunk"] = 1},
	health = 10,
	mass = 1.5,
	stacking = true}

Itemspec{
	name = "granite stone",
	model = "granitestone1",
	icon = "stone1",
	categories = {"build", "material"},
	construct_tile = "granite1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 5,
	stacking = true}

Itemspec{
	name = "iron ingot",
	model = "book-000", -- FIXME
	icon = "ingot1",
	mass = 2,
	categories = {"build", "material"},
	construct_tile = "iron1",
	construct_tile_count = 1,
	crafting_materials = {["ferrostone"] = 1},
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "hide",
	model = "book-000", -- FIXME
	icon = "hide1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "dragon scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "lizard scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "log",
	model = "log1",
	icon = "log1",
	categories = {"build", "material"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	construct_tile = "wood1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 50,
	mass = 5,
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "another log",
	model = "log2",
	icon = "log1",
	categories = {"build", "material","throwable"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	construct_tile = "wood1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 50,
	mass = 5,
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "milky powder",
	model = "book-000", -- FIXME
	icon = "potion3",
	categories = {"material"},
	crafting_count = 20,
	crafting_materials = {["mushroom"] = 1},
	mass = 0.05,
	stacking = true}

Itemspec{
	name = "mushroom",
	model = "mushroom-000",--"mushroom-001"..."mushroom-008"
	icon = "mushroom1",
	categories = {"material"},
	mass = 1,
	stacking = true}

Itemspec{
	name = "sand chunk",
	model = "sandchunk1",
	icon = "sand1",
	categories = {"build", "material"},
	construct_tile = "sand1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 3,
	stacking = true}

Itemspec{
	name = "soil chunk",
	model = "soilchunk1",
	icon = "dirt1",
	categories = {"build", "material"},
	construct_tile = "soil1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 3,
	stacking = true}

Itemspec{
	name = "wheat",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

------------------------------------------------------------------------------
-- Ammo.

Itemspec{
	name = "arrow",
	model = "arrow-000",
	categories = {"arrow"},
	influences_base = {physical = -5},
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	icon = "arrow1",
	mass = 0.2,
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "fire arrow",
	model = "arrow-000",
	categories = {"arrow"},
	influences_base = {physical = -5},
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	icon = "arrow1",
	mass = 1,
	stacking = true,
	special_effects = {
		{slot = "root", model = "torchfx1"}},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "explosive arrow",
	model = "arrow-000",
	categories = {"arrow","explosive"},
	influences_base = {physical = -5},
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	destroy_timer = 5,
	destroy_actions = {"explode"},
	icon = "arrow1",
	mass = 1,
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "bullet",
	model = "arrow-000", -- FIXME
	mass = 0.2,
	categories = {"bullet"},
	influences_base = {physical = -1},
	crafting_count = 10,
	crafting_materials = {["iron ingot"] = 1, ["crimson stone"] = 1},
	icon = "arrow1",
	stacking = true}

------------------------------------------------------------------------------
-- Miscellaneous.

Itemspec{
	name = "empty bottle",
	model = "emptypotion1",
	categories = {"misc"},
	crafting_materials = {["glass ingot"] = 1},
	health = 5,
	mass = 1,
	icon = "potion3", -- FIXME
	stacking = true}

Itemspec{
	name = "torch",
	model = "torch1",
	animation_hold = "hold torch",
	categories = {"misc"},
	crafting_materials = {["log"] = 1},
	effect_equip = "torch1",
	effect_unequip = "torch1",
	equipment_slot = "hand.L",
	icon = "torch1",
	mass = 3,
	special_effects = {
		{slot = "flame", model = "torchfx1"}},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "impstone",
	model = "firestone1",
	icon = "stone1",
	equipment_slot = "hand.R",
	categories = {"melee", "weapon"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	influences_base = {physical = -1},
	mass = 4,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "firestone",
	icon = "stone2",
	model = "firestone1",
	categories = {"explosive", "melee", "weapon"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	influences_base = {fire = -10, physical = -10},
	influences_bonus = {strength = 0.01},
	destroy_actions = {"explode"},
	destroy_timer = 5,
	equipment_slot = "hand.R",
	health = 1,
	mass = 4,
	special_effects = {
		{slot = "flame", model = "torchfx1"}},
	stacking = false}

------------------------------------------------------------------------------
-- Containers.

Itemspec{
	name = "chest",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	animations = {
		["loot"] = {animation = "opening", channel = 1, permanent = true, repeat_start = 10},
		["looted"] = {animation = "open", channel = 1, permanent = true, repeat_start = 10, time = 10}},
	categories = {"container", "generate"},
	crafting_materials = {log = 10},
	effect_craft = "craftwood1",
	inventory_size = 8,
	inventory_type = "chest",
	loot_categories = {"armor", "book", "material", "misc", "potion", "shield", "weapon"},
	loot_count = {2, 4},
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "jar",
	model = "jar1",
	icon = "potion3", -- FIXME
	mass = 9,
	categories = {"container", "generate"},
	crafting_materials = {["granite stone"] = 3},
	inventory_size = 4,
	inventory_type = "chest",
	loot_categories = {"material", "misc", "potion"},
	loot_count = {0, 4},
	stacking = false}

Itemspec{
	name = "workbench",
	model = "workbench1",
	icon = "workbench1",
	mass = 21,
	animation_looted = "open",
	animation_looting = "opening",
	categories = {"container", "workbench"},
	crafting_materials = {log = 7},
	description = "Right click and drag materials inside to craft.",
	effect_craft = "craftwood1",
	inventory_size = 4,
	inventory_type = "workbench",
	stacking = false,
	water_gravity = Vector(0,6,0)}

------------------------------------------------------------------------------
-- Decorations

Itemspec{
	name = "chair",
	icon = "chest1", -- FIXME
	model = "chair1",
	mass = 10,
	categories = {"furniture"},
	crafting_materials = {log = 5},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wood chair",
	model = "chair2",
	icon = "chest1", -- FIXME
	mass = 10,
	categories = {"furniture"},
	crafting_materials = {log = 5},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wood bench",
	model = "bench1",
	icon = "chest1", -- FIXME
	mass = 15,
	categories = {"furniture"},
	crafting_materials = {log = 6},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wood table",
	model = "table1",
	icon = "chest1", -- FIXME
	mass = 20,
	categories = {"furniture"},
	crafting_materials = {log = 7},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "broken door",
	model = "door1",
	icon = "chest1", -- FIXME
	mass = 20,
	categories = {"furniture"},
	crafting_materials = {log = 7},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

------------------------------------------------------------------------------
-- FIXME

Itemspec{
	name = "air tank",
	model = "airtank1",
	mass = 15,
	categories = {"explosive"},
	crafting_materials = {log = 1},
	destroy_actions = {"explode"},
	effect_craft = "craftmetal1",
	health = 1,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "gem",
	icon = "gem1",
	model = "gem-000",
	mass = 2,
	categories = {"reagent"},
	stacking = true}

Itemspec{
	name = "health potion",
	model = "potion3",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {["empty bottle"] = 1, ["milky powder"] = 1},
	crafting_skills = {intelligence = 50},
	description = "Right click to heal yourself.",
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	stacking = true,
	icon = "potion1",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "mana potion",
	model = "potion7",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {["empty bottle"] = 1, ["milky powder"] = 3},
	crafting_skills = {intelligence = 50},
	description = "Right click to replenish your willpower.",
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	stacking = true,
	icon = "potion2",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "Steamworks",
	icon = "scroll1",
	model = "book-000",
	mass = 2,
	categories = {"book"},
	book_text =
[[Steam-powered devices have brought a world of new opportunities to the aer society. The technology is still young but it has shown lots of promise in automating tasks. There is even progress in creating advanced devices that are able to reason and act on their own.

Steam technology relies on mechanical parts that are brought into motion by steam circulating in a system of pipes. The steam was initially created by conventional heating methods but more compact and efficient methods based on elemental forces such as ki were discovered very quickly.

With the power of this new technology on their side, the aer society are confident that they can defeat the unknown forces behind the withering of the World Tree.]]}
