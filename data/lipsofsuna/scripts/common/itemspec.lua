require "common/spec"

Itemspec = Class(Spec)
Itemspec.type = "item"
Itemspec.dict_id = {}
Itemspec.dict_cat = {}
Itemspec.dict_name = {}

--- Creates a new item specification.
-- @param clss Itemspec class.
-- @param args Arguments.<ul>
--   <li>ammo_type: Name of the ammunition item type.</li>
--   <li>animation_attack: Attack animation name for creatures wielding the item.</li>
--   <li>animation_hold: Hold animation name for creatures wielding the item.</li>
--   <li>animations: Dictionary of animation playback rules.</li>
--   <li>armor_class: How much protection the item offers when equipped.</li>
--   <li>categories: List of categories to which the item belongs.</li>
--   <li>construct_tile: Tile type to construct when used.</li>
--   <li>construct_tile_count: Material count required by tile construction.</li>
--   <li>crafting_count: Number of items crafted per use.</li>
--   <li>crafting_materials: Dictionary of crafting materials.</li>
--   <li>crafting_skills: Dictionary of required crafting skills.</li>
--   <li>influnces_base: Base influences.</li>
--   <li>influences_bonus: Dictionary of skill names and influence multipliers.</li>
--   <li>destroy_actions: List of actions to perform when the item is destroyed.</li>
--   <li>destroy_timer: Time in seconds after which to destruct when thrown.</li>
--   <li>effect_attack: Name of the effect to play when the item is used for attacking.</li>
--   <li>effect_craft: Name of the effect to play when the item is crafted.</li>
--   <li>effect_equip: Name of the effect to play when the item is equiped.</li>
--   <li>effect_unequip: Name of the effect to play when the item is unequiped.</li>
--   <li>effect_use: Name of the effect to play when the item is used.</li>
--   <li>equipment_models: Dictionary of equipment models.</li>
--   <li>equipment_slot: Equipment slot into which the item can be placed.</li>
--   <li>equipment_slots_reserved: List of equipment slots that the item reserves in addition to the main slot.</li>
--   <li>health: Number of hit points the item has.</li>
--   <li>icon: Icon name.</li>
--   <li>interactive: False to make the object not appear interactive.</li>
--   <li>inventory_items: List of inventory items for containers.</li>
--   <li>inventory_size: Size of the inventory, makes the item a container.</li>
--   <li>inventory_type: Inventory type string.</li>
--   <li>loot_categories: List of item categories this container can have as random loot.</li>
--   <li>loot_count: Minimum and maximum number of random loot items this container can have.</li>
--   <li>mass: Mass in kilograms.</li>
--   <li>model: Model to use for the item.</li>
--   <li>name: Name of the item type.</li>
--   <li>special_effects: List of special effects to render.</li>
--   <li>stacking: True to allow the item to stack in the inventory.</li></ul>
-- @return New item specification.
Itemspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.crafting_enabled = (self.crafting_count or self.crafting_materials or self.crafting_skills) and true
	-- Reserved equipment slots.
	if args.equipment_slots_reserved then
		self.equipment_slots_reserved = {}
		for k,v in pairs(args.equipment_slots_reserved) do
			self.equipment_slots_reserved[v] = true
		end
	end
	-- Models.
	-- The table, if present, maps a list of models for one or more species.
	-- The race name can contain multiple races separated by a slash.
	if args.equipment_models then
		self.equipment_models = {}
		for races,models in pairs(args.equipment_models) do
			for _,race in pairs(string.split(races, "/")) do
				self.equipment_models[race] = models
			end
		end
	end
	-- Default values.
	local copy = function(f, d) if self[f] == nil then self[f] = d end end
	copy("animations", {})
	copy("armor_class", 0)
	copy("crafting_count", 1)
	copy("crafting_materials", {})
	copy("crafting_skills", {})
	copy("destroy_actions", {})
	copy("effect_attack")
	copy("effect_craft")
	copy("effect_equip")
	copy("effect_unequip")
	copy("effect_use")
	copy("gravity", Vector(0,-15,0))
	copy("interactive", true)
	copy("inventory_items", {})
	copy("mass", 10)
	copy("special_effects", {})
	copy("water_friction", 0.9)
	copy("water_gravity", Vector(0,-3,0))
	return self
end

--- Finds the equipment models for the race.
-- @param self Itemspec.
-- @param name Name of the equipment class matching the race.
-- @param lod True for low level of detail.
-- @return Table of equipment models or nil.
Itemspec.get_equipment_models = function(self, name, lod)
	-- Choose the level of detail.
	-- If the requested level doesn't exist, fall back to the other one.
	local models = nil
	if lod then models = self.equipment_models_lod end
	if not models then models = self.equipment_models end
	if not models then models = self.equipment_models_lod end
	-- Find the equipment models for the race.
	return models and models[name]
end
