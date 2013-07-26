Unittest:add(1, "system", "filter: fir", function()
	local FirFilter = require("system/filter/fir")
	local f = FirFilter{1,-1,1,-1,2}
	assert(f)
	f:add(100)
	assert(f:get() == 100)
	f:add(0)
	assert(f:get() == -100)
	f:add(0)
	assert(f:get() == 100)
	f:add(0)
	assert(f:get() == -100)
	f:add(1)
	assert(f:get() == 201)
	f:add(0)
	assert(f:get() == -1)
end)
