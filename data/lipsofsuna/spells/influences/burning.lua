-- Sets the target on fire.
Feateffectspec:extend{
	name = "burning",
	modifier = function(self, mod, secs)
		-- Update the burning timer.
		mod.timer = mod.timer + secs
		-- Damage the object every second.
		if mod.timer > 1 then
			mod.object:damaged{amount = math.random(4,7), type = "burning"}
			mod.timer = mod.timer - 1
		end
		-- End after the timeout.
		mod.strength = mod.strength - secs
		return mod.strength > 0
	end,
	touch = function(self, args)
		if not args.object then return end
		args.object:inflict_modifier("burning", args.value)
	end,
	ranged = function(self, args)
		self:touch(args)
	end}