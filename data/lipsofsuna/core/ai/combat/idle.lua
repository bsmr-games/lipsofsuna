local AiActionSpec = require("core/specs/aiaction")

AiActionSpec{
	name = "idle",
	categories = {["combat"] = true, ["defensive"] = true},
	calculate = function(self, args)
		return 0.0000001
	end,
	perform = function(self, args)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.action_timer = math.random(1, 5)
	end}
