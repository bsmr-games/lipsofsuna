return {
	name = "number",
	equals = function(self, val1, val2)
		return val1 == val2
	end,
	validate = function(self, val)
		return type(val) == "number"
	end,
	write_str = function(self, val)
		return tostring(val)
	end}
