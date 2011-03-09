------------------------------------------------------------------------------
-- Materials.

Itemspec{
	name = "adamantium",
	model = "adamantium1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "aquanite stone",
	model = "aquanitestone1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "basalt stone",
	model = "basaltstone1",
	mass = 1,
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
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "dragon scale",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "ferrostone",
	model = "ferrostone1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "ferrostone1",
	construct_tile_count = 1,
	crafting_materials = {["granite stone"] = 2},
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
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "glass ingot",
	model = "book-000", -- FIXME
	mass = 3,
	categories = {"material"},
	crafting_materials = {["sand chunk"] = 1},
	stacking = true}

Itemspec{
	name = "granite stone",
	model = "granitestone1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "granite1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "iron ingot",
	model = "book-000", -- FIXME
	mass = 3,
	categories = {"build", "material"},
	construct_tile = "iron1",
	construct_tile_count = 1,
	crafting_materials = {["ferrostone"] = 1},
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "hide",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "lizard scale",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "log",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "wood1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "milky powder",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	crafting_count = 20,
	crafting_materials = {["mushroom"] = 1},
	stacking = true,
	icon = "mushroom1"}

Itemspec{
	name = "mushroom",
	model = "mushroom-000",--"mushroom-001"..."mushroom-008"
	mass = 1,
	categories = {"material"},
	stacking = true,
	icon = "mushroom1"}

Itemspec{
	name = "sand chunk",
	model = "sandchunk1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "sand1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "soil chunk",
	model = "soilchunk1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "soil1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "wheat",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

------------------------------------------------------------------------------
-- Weapons.

Itemspec{
	name = "adamantium crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "adamantium dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 20,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "adamantium hatchet",
	model = "axe1",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-axe", channel = "attack-right"},
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 20,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "adamantium mattock",
	model = "mattock-000",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-blunt", channel = "attack-right"},
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	damage = 20,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "adamantium musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 15, ["log"] = 2},
	damage = 50,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "adamantium sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 40,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "aquanite crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	damage = 22,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "aquanite dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "aquanite grenade",
	model = "grenade1",
	mass = 3,
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	damage = 40,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	stacking = true}

Itemspec{
	name = "aquanite hatchet",
	model = "axe1",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-axe", channel = "attack-right"},
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 15,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "aquanite mattock",
	model = "mattock-000",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-blunt", channel = "attack-right"},
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	damage = 15,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "aquanite musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 15, ["log"] = 2},
	damage = 37,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "aquanite sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "crimson crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	damage = 22,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "crimson dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "crimson grenade",
	model = "grenade1",
	mass = 3,
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	damage = 40,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	stacking = true}

Itemspec{
	name = "crimson hatchet",
	model = "axe1",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-axe", channel = "attack-right"},
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 15,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "crimson mattock",
	model = "mattock-000",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-blunt", channel = "attack-right"},
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	damage = 15,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "crimson musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 15, ["log"] = 2},
	damage = 37,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "crimson sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "iron crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "iron dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "iron hatchet",
	model = "axe1",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage = 10,
	damage_bonus_strength = 0.2,
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-axe", channel = "attack-right"},
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "iron mattock",
	model = "mattock-000",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage = 10,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "iron musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 5, ["log"] = 2},
	damage = 25,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "iron sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "wooden crossbow",
	model = "bow1", -- FIXME
	animation_attack = {animation = "fire-bow", channel = "attack-right"},
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 3},
	damage = 7,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "wooden dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 5,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "wooden hatchet",
	model = "axe1",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-axe", channel = "attack-right"},
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 5,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "wooden mattock",
	model = "mattock-000",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-blunt", channel = "attack-right"},
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["log"] = 3},
	damage = 5,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "wooden sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "wooden club",
	model = "woodenclub1",
	animation_hold = {animation = "hold-blunt", channel = "equip-right"},
	animation_attack = {animation = "attack-blunt", channel = "attack-right"},
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 15,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

------------------------------------------------------------------------------
-- Armor.

-- TODO

Itemspec{
	name = "dress",
	model = "chinadress1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {log = 1},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerchinadress1", panties = "panties2"},
		["android"] = {upper1 = "androidchinadress1"},
		["devora"] = {upper1 = "devorachinadress1"}},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "trog rags",
	model = "book-000",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {log = 1},
	equipment_models = {
		["troglodyte female"] = {upper1 = "trogfemrags1"}},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "iron greaves",
	model = "book-000",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 3},
	equipment_models = {
		["aer/kraken/wyrm"] = {legs = "aerlegplate1"}},
	equipment_slot = "feet"}

Itemspec{
	name = "iron gauntlets",
	model = "book-000",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 2},
	equipment_models = {
		["aer/kraken/wyrm"] = {arms = "aerarmplate2"}},
	equipment_slot = "arms"}

Itemspec{
	name = "iron breastplate",
	model = "book-000",
	armor_class = 0.18,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 5},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper = "aerupperplate1", lower = "aerlowerplate1"}},
	equipment_slot = "upperbody"}

Itemspec{
	name = "kitty headband",
	model = "headband1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {hide = 2},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {headband = "headband1"}},
	equipment_slot = "head"}

Itemspec{
	name = "leather helm",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 2},
	equipment_models = {
		["aermale"] = {hair = "aermale1leatherhead2"}},
	equipment_slot = "head"}

Itemspec{
	name = "leather leggings",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {legs = "aerlegarmor1"}},
	equipment_slot = "feet"}


Itemspec{
	name = "leather gloves",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {["hide"] = 2},
	equipment_models = {
		["aermale"] = {hands = "aermale1leatherarms2"}},
	equipment_slot = "arms"}

Itemspec{
	name = "leather boots",
	model = "boots1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {legs = "aerlegarmor2", legs1 = "boots1"},["aermale"] = {feet = "aermale1boots1"}},
	equipment_slot = "feet"}

Itemspec{
	name = "leather pants",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerlowerarmor1", panties = "panties2"},["aermale"] = {legs = "aermale1leatherlegs2"}},
	equipment_slot = "lowerbody"}

Itemspec{
	name = "leather brigand pants",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerlowerarmor1", panties = "panties2"},["aermale"] = {legs = "aermale1leatherlegs1"}},
	equipment_slot = "lowerbody"}
Itemspec{
	name = "leather top",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "bra1"}},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "leather skirt",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerleatherskirt1", panties = "panties2"}},
	equipment_slot = "lowerbody"}

Itemspec{
	name = "lizard boots",
	model = "boots2",
	armor_class = 0.5,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {legs1 = "aerboots2"}},
	equipment_slot = "feet"}

Itemspec{
	name = "lizard dress",
	model = "book-000",
	armor_class = 0.1,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 6},
	equipment_models = {
		["aer/kraken/wyrm"] = {dress1 = "aerdress2", panties = "panties2"}},
	equipment_slot = "upperbody"}

Itemspec{
	name = "lizard gloves",
	model = "gloves2",
	armor_class = 0.5,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {arms1 = "aergloves2"}},
	equipment_slot = "arms"}

Itemspec{
	name = "leather chest",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerleatherchest1"},["aermale"] = {upper = "aermale1leatherchest2"}},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "leather brigand chest",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerleatherchest1"},["aermale"] = {upper = "aermale1leatherchest1"}},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "round shield",
	model = "shield1",
	animation_hold = {animation = "hold-left", channel = "equip-left"},
	armor_class = 0.08,
	categories = {"shield"},
	crafting_materials = {["log"] = 5, ["iron ingot"] = 3},
	equipment_slot = "hand.L"}

Itemspec{
	name = "wooden shield",
	model = "shield-001-wood1",
	animation_hold = {animation = "hold-left", channel = "equip-left"},
	armor_class = 0.08,
	categories = {"shield"},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L"}

Itemspec{
	name = "shirt",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	equipment_models = {
		["aermale"] = {upper = "aermale1shirt1"}},
	equipment_slot = "upperbody",
	icon = "cloth1"}
Itemspec{
	name = "pants",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	equipment_models = {
		["aermale"] = {legs = "aermale1pants1"}},
	equipment_slot = "lowerbody",
	icon = "cloth1"}

------------------------------------------------------------------------------
-- Miscellaneous.

Itemspec{
	name = "arrow",
	model = "arrow-000",
	mass = 1,
	categories = {"arrow"},
	damage = 5,
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	icon = "arrow1",
	stacking = true}

Itemspec{
	name = "bullet",
	model = "arrow-000", -- FIXME
	mass = 1,
	categories = {"bullet"},
	damage = 5,
	crafting_count = 10,
	crafting_materials = {["iron ingot"] = 1, ["crimson stone"] = 3},
	icon = "arrow1",
	stacking = true}

Itemspec{
	name = "torch",
	model = "torch1",
	animation_hold = {animation = "hold-left", channel = "equip-left"},
	categories = {"misc"},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L",
	icon = "torch1"}

------------------------------------------------------------------------------
-- Containers.

Itemspec{
	name = "chest",
	model = "chest1",
	mass = 100,
	animation_looted = "open",
	animation_looting = "opening",
	categories = {"container", "generate"},
	crafting_materials = {log = 1},
	inventory_size = 4,
	inventory_type = "chest",
	loot_categories = {"armor", "book", "material", "misc", "potion", "shield", "weapon"},
	loot_count = {2, 4},
	stacking = false}

Itemspec{
	name = "jar",
	model = "jar1",
	mass = 10,
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
	mass = 15,
	animation_looted = "open",
	animation_looting = "opening",
	categories = {"container", "workbench"},
	crafting_materials = {log = 1},
	inventory_size = 4,
	inventory_type = "workbench",
	stacking = false}

------------------------------------------------------------------------------
-- Decorations

Itemspec{
	name = "chair",
	model = "chair1",
	mass = 10,
	categories = {"furniture"},
	crafting_materials = {log = 5},
	health = 30,
	stacking = false}

------------------------------------------------------------------------------
-- FIXME

Itemspec{
	name = "air tank",
	model = "airtank1",
	mass = 15,
	categories = {"explosive"},
	crafting_materials = {log = 1},
	destroy_actions = {"explode"},
	health = 1,
	stacking = false}

Itemspec{
	name = "boomerang",
	model = "boomerang1",
	mass = 3,
	categories = {"boomerang", "throwable", "weapon"},
	crafting_materials = {log = 1},
	damage = 20,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "gem",
	model = "gem-000",
	mass = 2,
	categories = {"reagent"},
	stacking = true}

Itemspec{
	name = "greater health potion",
	model = "potion3",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 75},
	stacking = true,
	icon = "potion1"}

Itemspec{
	name = "greater mana potion",
	model = "potion7",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 75},
	stacking = true,
	icon = "potion2"}

Itemspec{
	name = "health potion",
	model = "potion3",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 50},
	stacking = true,
	icon = "potion1"}

Itemspec{
	name = "lesser health potion",
	model = "potion3",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 25},
	stacking = true,
	icon = "potion1"}

Itemspec{
	name = "lesser mana potion",
	model = "potion7",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 25},
	stacking = true,
	icon = "potion2"}

Itemspec{
	name = "mana potion",
	model = "potion7",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 50},
	stacking = true,
	icon = "potion2"}

Itemspec{
	name = "Steamworks",
	model = "book-000",
	mass = 2,
	categories = {"book"},
	book_text =
[[Steam-powered devices have brought a world of new opportunities to the aer society. The technology is still young but it has shown lots of promise in automating tasks. There is even progress in creating advanced devices that are able to reason and act on their own.

Steam technology relies on mechanical parts that are brought into motion by steam circulating in a system of pipes. The steam was initially created by conventional heating methods but more compact and efficient methods based on elemental forces such as ki were discovered very quickly.

With the power of this new technology on their side, the aer society are confident that they can defeat the unknown forces behind the withering of the World Tree.]]}
