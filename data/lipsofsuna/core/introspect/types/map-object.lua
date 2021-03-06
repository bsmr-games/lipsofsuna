return {
	name = "map object",
	equals = function(self, val1, val2)
		return val1[1] == val2[1] and
		       val1[2] == val2[2] and
		       val1[3] == val2[3] and
		       val1[4] == val2[4] and
		       val1[5] == val2[5]
	end,
	write_str = function(self, val)
		if val[5] then
			return string.format("{%f,%f,%f,%q,%.2f}",
				val[1], val[2], val[3], val[4], val[5])
		else
			return string.format("{%f,%f,%f,%q}",
				val[1], val[2], val[3], val[4])
		end
	end}
