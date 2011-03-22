require "common/spec"

Species = Class(Spec)
Species.type = "species"
Species.dict_id = {}
Species.dict_cat = {}
Species.dict_name = {}

--- Registers a new species.
-- @param clss Species class.
-- @param args Arguments.<ul>
--   <li>ai_distance_hint
--   <li>ai_enable_attack False to prohibit attacking.</li>
--   <li>ai_enable_combat False to prohibit combat.</li>
--   <li>ai_enable_defend False to prohibit defending.</li>
--   <li>ai_enable_strafe False to prohibit strafing.</li>
--   <li>ai_enable_wander False to prohibit wandering.</li>
--   <li>ai_enable_walk: False to prohibit walking.</li>
--   <li>ai_enabled: False to disable AI for the species.</li>
--   <li>ai_search_time: Time in seconds how long the AI will search for hidden foes.</li>
--   <li>ai_update_delay: Number of seconds between AI state updates.</li>
--   <li>ai_wait_allowed: False to prohibit waiting.</li>
--   <li>ai_wander_time: Time in seconds how long the AI will wander before going to sleep.</li>
--   <li>aim_ray_center: Center vector of the aim ray.</li>
--   <li>aim_ray_end: Aim ray end distance.</li>
--   <li>aim_ray_start: Aim ray start distance.</li>
--   <li>animations: Dictionary of animation playback rules.</li>
--   <li>blocking_armor: How much armor class blocking offsers.</li>
--   <li>blocking_cooldown: Time in seconds how long it takes to leave the blocking stance.</li>
--   <li>blocking_delay: Time in seconds how long it takes to enter the blocking stance.</li>
--   <li>damage_from_magma: Points of damage from magma per second.</li>
--   <li>damage_from_water: Points of damage from water per second.</li>
--   <li>effect_falling_damage: Name of the effect played when the creature takes falling damage.</li>
--   <li>effect_landing: Name of the effect played when the creature lands after jumping.</li>
--   <li>equipment_slots: List of equipment slots.</li>
--   <li>factions: List of factions.</li>
--   <li>falling_damage_rate: Number of points of damage per every meters per second exceeding the falling damage speed.</li>
--   <li>falling_damage_speed: Speed in meters of seconds after which the creature starts taking falling damage.</li>
--   <li>feat_animations: List of know feat types.</li>
--   <li>feat_effects: List of know feat effects.</li>
--   <li>gravity: Gravity vector.</li>
--   <li>inventory_items: List of inventory items to give when the creature is spawned.</li>
--   <li>inventory_size: Number of inventory slots the creature has.</li>
--   <li>jump_force: Mass-independent jump force of the creature.</li>
--   <li>loot_categories: List of item categories this creature can have as random loot.</li>
--   <li>loot_count: Minimum and maximum number of random loot items this creature can have.</li>
--   <li>mass: Mass in kilograms.</li>
--   <li>model: Model name.</li>
--   <li>skill_quota: Number of skill points the creature can distribute over skills.</li>
--   <li>skills: List of skills.</li>
--   <li>special_effects: List of special effects to render.</li>
--   <li>speed_run: Turning speed in meters per second.</li>
--   <li>speed_walk: Walking speed in meters per second.</li>
--   <li>swim_force: Mass-independent upward swim force of the creature.</li>
--   <li>tilt_bone: Name of the tilt bone of the creature.</li>
--   <li>tilt_limit: Tilt limit in radians of the tilting bone.</li>
--   <li>timing_attack_blunt: Timing of blunt attack impact, in frames.</li>
--   <li>timing_attack_explode: Timing of explosion attack, in frames.</li>
--   <li>timing_attack_ranged: Timing of ranged attack firing, in frames.</li>
--   <li>timing_attack_throw: Timing of releasing the thrown item, in frames.</li>
--   <li>timing_build: Timing of releasing the thrown item, in frames.</li>
--   <li>timing_jump: Timing of lifting off when jumping, in frames.</li>
--   <li>timing_spell_ranged: Timing of casting a ranged spell projectile, in frames.</li>
--   <li>timing_spell_self: Timing of casting a spell on self, in frames.</li>
--   <li>timing_spell_touch: Timing of casting a spell on touch, in frames.</li>
--   <li>view_cone: View cone angle in radians.</li>
--   <li>water_friction: How much being in water slows the creature down.</li>
--   <li>water_gravity: The gravity of the creature in water.</li></ul>
-- @return New species.
Species.new = function(clss, args)
	-- Copy arguments from the base species.
	-- The arguments used to initialize each species have been stored to the
	-- args field of the instance so that they can be easily copied to the
	-- inherited species here.
	local base = args.base and Species:find{name = args.base}
	if base then
		local t = {}
		for k,v in pairs(base.args) do t[k] = v end
		for k,v in pairs(args) do t[k] = v end
		args = t
	end
	-- Allocate the species.
	-- Many arguments are copied directly from the argument list but some
	-- are reset to empty arrays because they require conversion that is
	-- done later. The argument list is also stored to the species to
	-- allow easy inheritance.
	local self = Spec.new(clss, args)
	self.args = args
	-- Animations.
	-- Inheritance of animations is special in that they're inherited
	-- in per animation basis instead of the usual all or none inheritance.
	-- This allows species to replace individual animations easily.
	self.animations = {}
	if base and base.animations then
		for k,v in pairs(base.animations) do
			self.animations[k] = v
		end
	end
	if args.animations then
		for k,v in pairs(args.animations) do
			self.animations[k] = v
		end
	end
	-- Equipment slots.
	-- Converted from a list to a dictionary to make searching easier.
	self.equipment_slots = {}
	if args.equipment_slots then
		for k,v in pairs(args.equipment_slots) do
			self.equipment_slots[v.name] = v
		end
	end
	-- Factions.
	-- Converted from a list to a dictionary to make searching easier.
	-- The faction names are also replaced by the faction objects themselves.
	self.factions = {}
	if args.factions then
		for k,v in pairs(args.factions) do
			self.factions[v] = Faction:find{name = v}
		end
	end
	-- Feats.
	-- Converted from a list to a dictionary to make searching easier.
	-- The key is the name of the feat and the value is unused.
	self.feat_anims = {}
	if args.feat_anims then
		for k,v in pairs(args.feat_anims) do
			self.feat_anims[v] = true
		end
	end
	self.feat_effects = {}
	if args.feat_effects then
		for k,v in pairs(args.feat_effects) do
			self.feat_effects[v] = true
		end
	end
	-- Skills.
	-- Converted from a list to a dictionary to make searching easier.
	self.skills = {}
	if args.skills then
		for k,v in pairs(args.skills) do
			self.skills[v.name] = v
		end
	end
	-- Default values.
	-- To make things easier for the rest of the scripts, some fields are
	-- guaranteed to always have a value. The default values are set here
	-- if no value is assigned to the field yet.
	local copy = function(f, d) if self[f] == nil then self[f] = d end end
	copy("ai_distance_hint", 1)
	copy("ai_enable_attack", true)
	copy("ai_enable_combat", true)
	copy("ai_enable_defend", true)
	copy("ai_enable_strafe", true)
	copy("ai_enable_wander", true)
	copy("ai_enable_walk", true)
	copy("ai_enabled", true)
	copy("ai_search_time", 20)
	copy("ai_update_delay", 2)
	copy("ai_wait_allowed", true)
	copy("ai_wander_time", 60)
	copy("aim_ray_center", Vector(0, 1, 0))
	copy("aim_ray_end", 5)
	copy("aim_ray_start", 0.1)
	copy("blocking_armor", 0.5)
	copy("blocking_cooldown", 0.4)
	copy("blocking_delay", 0.4)
	copy("damage_from_magma", 6)
	copy("damage_from_water", 0)
	copy("falling_damage_rate", 3)
	copy("falling_damage_speed", 12)
	copy("gravity", Vector(0, -15, 0))
	copy("inventory_items", {})
	copy("inventory_size", 0)
	copy("jump_force", 8)
	copy("mass", 50)
	copy("skill_quota", 200)
	copy("special_effects", {})
	copy("speed_walk", 3)
	copy("speed_run", 6)
	copy("swim_force", 3.5)
	copy("tilt_limit", math.pi)
	copy("timing_attack_blunt", 10)
	copy("timing_attack_explode", 60)
	copy("timing_attack_ranged", 10)
	copy("timing_attack_throw", 10)
	copy("timing_build", 10)
	copy("timing_jump", 5)
	copy("timing_spell_ranged", 10)
	copy("timing_spell_self", 10)
	copy("timing_spell_touch", 10)
	copy("view_cone", 0.8 * math.pi)
	copy("water_friction", 0.6)
	copy("water_gravity", Vector(0,-3,0))
	return self
end

--- Checks if the object is an enemy of the species.
-- @param self Species.
-- @param object Object.
-- @return True if an enemy.
Species.check_enemy = function(self, object)
	if object.spec.type == "species" then
		for name1,spec1 in pairs(self.factions) do
			for name2,spec2 in pairs(object.spec.factions) do
				if spec1.enemies[name2] then
					return true
				end
			end
		end
	end
end

--- Sets the factions of the species.
-- @param self Species.
-- @param args List of species.
Species.set_factions = function(self, args)
	if args then
		self.factions = {}
		for k,v in pairs(args) do
			self.factions[v] = Faction:find{name = v}
		end
	end
end
