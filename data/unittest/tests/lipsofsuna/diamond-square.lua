Unittest:add(1, "lipsofsuna", "diamond-square", function()
	local DiamondSquare = require("landscape/generator/diamond-square")
	local ds = DiamondSquare(16)
	ds:calculate({1000, 2000})
end)
