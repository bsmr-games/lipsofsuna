Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "lizard boots",
	effect = "craftsewing1",
	materials = {["lizard scale"] = 4}}

Itemspec{
	name = "lizard boots",
	categories = {["armor"] = true},
	armor_class = 0.05,
	equipment_models = {
		["aer"] = {feet = "aerlizardboots1"}},
	equipment_slot = "feet",
	icon = "lizardboot1",
	mass = 4,
	model = "lizardboots1",
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "lizard dress",
	effect = "craftsewing1",
	materials = {["lizard scale"] = 6}}

Itemspec{
	name = "lizard dress",
	categories = {["armor"] = true},
	armor_class = 0.1,
	equipment_models = {
		["aer"] = {dress1 = "aerlizarddress1", upper_safe = ""},
		["kraken"] = {dress1 = "aerlizarddress1"}},
	equipment_slot = "upperbody",
	icon = "lizardarmor1",
	mass = 7,
	model = "lizarddress1",
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "lizard gloves",
	effect = "craftsewing1",
	materials = {["lizard scale"] = 4}}

Itemspec{
	name = "lizard gloves",
	categories = {["armor"] = true},
	armor_class = 0.05,
	equipment_models = {
		["aer"] = {hands = "aerlizardgloves1"},
		["kraken"] = {hands = "aerlizardgloves1"}},
	equipment_slot = "arms",
	icon = "lizardglove1",
	mass = 4,
	model = "lizardgloves1",
	water_gravity = Vector(0,6,0)}
