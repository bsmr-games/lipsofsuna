Unittest:add(1, "system", "json: decoding", function()
	local Json = require("system/json")
	local r1 = Json:decode("[{\"a\": true},1,2,\"b\",null,false]")
	assert(#r1 == 6)
	assert(type(r1[1]) == "table")
	assert(r1[1]["a"] == true)
	assert(r1[2] == 1)
	assert(r1[3] == 2)
	assert(r1[4] == "b")
	assert(r1[5] == nil)
	assert(r1[6] == false)
end)

Unittest:add(1, "system", "json: decoding error", function()
	local Json = require("system/json")
	local r1,err = Json:decode("[syntax]")
	assert(r1 == nil)
	assert(type(err) == "string")
end)

Unittest:add(1, "system", "json: encoding", function()
	local Json = require("system/json")
	local r1 = Json:encode({1, 2, 3, 4, 5})
	assert(r1 == "[1,2,3,4,5]")
end)