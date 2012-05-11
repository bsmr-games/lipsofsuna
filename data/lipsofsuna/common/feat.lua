Feat = Class()

--- Creates a new feat.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--   <li>animation: Feat type name.</li>
--   <li>effects: List of effects and their magnitudes.</li></ul>
-- @return New feat.
Feat.new = function(clss, args)
	local self = Class.new(clss, args)
	self.effects = self.effects or {}
	return self
end

--- Creates a copy of the feat.
-- @param feat Feat.
-- @return Feat.
Feat.copy = function(feat)
	local self = Class.new(Feat)
	self.animation = feat.animation
	self.effects = {}
	for k,v in pairs(feat.effects) do self.effects[k] = v end
	return self
end

--- Gets the icon of the feat.
-- @param self Feat.
-- @return Iconspec or nil.
Feat.get_icon = function(self)
	local icon = nil
	for i = 1,3 do
		local effect = self.effects[i]
		if effect then
			spec = Feateffectspec:find{name = effect[1]}
			icon = spec and Iconspec:find{name = spec.icon}
			if icon then break end
		end
	end
	return icon
end

--- Gets the skill and reagent requirements of the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>charge: Charge time of the attack.</li>
--   <li>object: Attacked object, or nil.</li>
--   <li>owner: Attacking actor.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Fired object.</li>
--   <li>weapon: Used weapon.</li></ul>
-- @return Feat info table.
Feat.get_info = function(self, args)
	local damage = 0
	local reagents = {}
	local stats = {}
	local influences = {}
	local health_influences = {["cold damage"] = 1, ["fire damage"] = 1, ["physical damage"] = 1, poison = 1}
	-- Get the feat type.
	local anim = Feattypespec:find{name = self.animation}
	if not anim then return end
	local cooldown = anim.cooldown
	-- Influence contribution.
	for n,v in pairs(anim.influences) do
		influences[n] = (influences[n] or 0) + v
		if influences[n] == 0 then
			influences[n] = nil
		end
	end
	-- Effect contributions.
	for index,data in pairs(self.effects) do
		local effect = Feateffectspec:find{name = data[1]}
		if effect then
			-- Stat requirements.
			for stat,value in pairs(effect.required_stats) do
				local val = stats[stat] or 0
				stats[stat] = val + value
			end
			-- Reagent requirements.
			for reagent,value in pairs(effect.required_reagents) do
				local val = reagents[reagent] or 0
				reagents[reagent] = val + value
			end
			-- Cooldown contribution.
			cooldown = cooldown + effect.cooldown
			-- Influence contribution.
			for n,v in pairs(effect.influences) do
				influences[n] = (influences[n] or 0) + v
				if influences[n] == 0 then
					influences[n] = nil
				end
			end
		end
	end
	-- Ammo requirements.
	local ammo = args and args.weapon and args.weapon.spec.ammo_type
	-- Stat requirements.
	for k,v in pairs(stats) do
		stats[k] = math.max(1, math.floor(v))
	end
	-- Reagent requirements.
	for k,v in pairs(reagents) do
		reagents[k] = math.max(1, math.floor(v))
	end
	-- Add weapon-specific influences.
	-- In addition to the base influences, weapons may grant bonuses for having
	-- points in certain skills. The skill bonuses are multiplicative since the
	-- system is easier to balance that way.
	if args and args.weapon and anim.bonuses_weapon and args.weapon.spec.influences then
		local mult = 1
		if args.owner.skills then
			mult = args.owner.skills:calculate_damage_multiplier_for_item(args.weapon)
		end
		for k,v in pairs(args.weapon.spec.influences) do
			influences[k] = (influences[k] or 0) + mult * v
		end
	end
	-- Add bare-handed-specific influences.
	-- Works like the weapon variant but uses hardcoded attributes and base damage.
	if args and not args.weapon and anim.bonuses_barehanded then
		local mult = 1
		if args.owner.skills then
			mult = args.owner.skills:calculate_damage_multiplier_for_unarmed()
		end
		local bonuses = {["physical damage"] = 3}
		for k,v in pairs(bonuses) do
			influences[k] = (influences[k] or 0) + mult * v
		end
	end
	-- Add projectile-specific influences.
	-- Works like the weapon variant but uses the projectile as the item.
	if args and args.projectile and anim.bonuses_projectile and args.projectile.spec.influences then
		local mult = 1
		if args.owner.skills then
			mult = args.owner.skills:calculate_damage_multiplier_for_item(args.weapon)
		end
		for k,v in pairs(args.projectile.spec.influences) do
			influences[k] = (influences[k] or 0) + mult * v
		end
	end
	-- Add berserk bonus.
	-- The bonus increases physical damage if the health of the owner is
	-- lower than 25 points. If the health is 1 point, the damage is tripled.
	if args and args.owner:get_modifier("berserk") then
		local p = influences["physical"]
		if anim.categories["melee"] and p and p < 0 then
			local h = args.owner.stats:get_value("health")
			local f = 3 - 2 * math.min(h, 25) / 25
			influences["physical"] = p * f
		end
	end
	-- Add charge bonus.
	-- Holding the attack button allows a power attack of a kind. The damage
	-- is doubled if charged to the maximum of 2 seconds.
	if args and args.charge then
		local p = influences["physical damage"]
		if (anim.categories["melee"] or anim.categories["ranged"]) and p and p > 0 then
			local f = 1 + math.min(1.5, args.charge / 2)
			influences["physical damage"] = p * f
		end
	end
	-- Apply object armor and blocking.
	-- Only a limited number of influence types is affected by this.
	-- Positive influences that would increase stats are never blocked.
	if args and args.object then
		local reduce = {["cold damage"] = true, ["fire damage"] = true, ["physical damage"] = true}
		local armor = args.object.armor_class or 0
		if args.object.blocking then
			local delay = args.object.spec.blocking_delay
			local elapsed = Program.time - args.object.blocking
			local frac = math.min(1, elapsed / delay)
			armor = armor + frac * args.object.spec.blocking_armor
		end
		local mult = math.max(0.0, 1 - armor)
		for k,v in pairs(influences) do
			if reduce[k] then
				local prev = influences[k]
				if prev < 0 then
					influences[k] = prev * mult
				end
			end
		end
	end
	-- Apply object vulnerabilities.
	-- Individual influences are multiplied by the vulnerability coefficients
	-- of the object and summed together to the total health influence.
	local vuln = args and args.object and args.object.spec.vulnerabilities
	if not vuln then vuln = health_influences end
	local health = influences.health or 0
	for k,v in pairs(influences) do
		local mult = vuln[k] or health_influences[k]
		if mult then
			local val = v * mult
			influences[k] = val 
			health = health + val
		end
	end
	-- Set the total health influence.
	-- This is the actual value added to the health of the object. The
	-- individual influence components remain in the table may be used
	-- by special feats but they aren't used for regular health changes.
	if health ~= 0 then
		influences.health = health
	end
	-- Return the final results.
	return {
		animation = anim,
		cooldown = cooldown,
		influences = influences,
		required_ammo = ammo and {[ammo] = 1} or {},
		required_reagents = reagents,
		required_stats = stats,
		required_weapon = anim and anim.required_weapon}
end

--- Creates a new feat from a data string.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--  <li>data: Data string.</li></ul>
-- @return Feat or nil.
Feat.load = function(clss, args)
	if not args.data then return end
	local handler = function(err) print(debug.traceback("ERROR: " .. err)) end
	-- Compile the string.
	local func,err1 = loadstring(args.data)
	if not func then return handler(err1) end
	-- Execute the code.
	local err2,ret = xpcall(func, handler)
	return ret
end

--- Saves the feat to a data string.
-- @param self Feat.
-- @return String.
Feat.write = function(self)
	return string.format("return Feat%s", serialize{
		animation = self.animation,
		effects = self.effects})
end