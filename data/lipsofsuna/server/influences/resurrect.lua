Feateffectspec:extend{
	name = "resurrect",
	touch = function(self, args)
		if not args.object then return end
		if args.object.class == Player and not args.object.client then
			args.owner:send("The corpse is devoid of life force...")
		else
			args.object:resurrect()
		end
	end,
	ranged = function(self, args)
		self:touch(args)
	end}
