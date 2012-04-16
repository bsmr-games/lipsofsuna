Dialogspec{name = "bigbuildings",
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch",
	{"choice","Build a wall in this direction:",
		{"choice","Cancel",{"break"}},
		{"choice","  East to West",	{"spawn pattern", "wallns",erase_tiles=true,position_relative=Vector(5,4,5),rotation=1},{"var","budilt"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "wallns",erase_tiles=true,position_relative=Vector(5,4,5)},{"var","built"},{"break"}},
		{"choice","  West to East",	{"spawn pattern", "wallns",erase_tiles=true,position_relative=Vector(5,4,5),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "wallns",erase_tiles=true,position_relative=Vector(5,4,5),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a diagonal wall in this direction:",
		{"choice","Cancel",{"break"}},
		{"choice","  East to West",	{"spawn pattern", "walldiagr",erase_tiles=true,position_relative=Vector(5,4,5),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "walldiagr",erase_tiles=true,position_relative=Vector(5,4,5)},{"var","built"},{"break"}},
		{"choice","  West to East",	{"spawn pattern", "walldiagr",erase_tiles=true,position_relative=Vector(5,4,5),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "walldiagr",erase_tiles=true,position_relative=Vector(5,4,5),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a wall node and tower",
		{"spawn pattern", "walltower", position_relative=Vector(5,4,5)},{"var","built"},{"break"}},
	{"choice","Build a space for small buildings",
		{"spawn pattern", "smallbuildingnodes", position_relative=Vector(5,2,5)},{"var","built"},{"break"}},
	{"choice", "Cancel",
		{"exit"}}},
	{"branch", check = {{"var", "built"}},
	{"func", function(q)
						--q.object:damaged{amount = 200, type = "physical"}
					end}},
	{"loop"}}
