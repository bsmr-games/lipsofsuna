Dialogspec{name = "smallbuildings",
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch",
	{"choice","Build a small wall in this direction:",
		{"choice","Cancel",{"break"}},
		{"choice","  East to West",	{"spawn pattern", "swallns",erase_tiles=true, position_relative=Vector(0,2,0), rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "swallns",erase_tiles=true, position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","  West to East",	{"spawn pattern", "swallns",erase_tiles=true, position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "swallns",erase_tiles=true, position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a small shop facing this direction:",
		{"choice","Cancel",{"break"}},
		{"choice","  East to West",	{"spawn pattern", "castleshop",erase_tiles=true, position_relative=Vector(0,2,0),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "castleshop",erase_tiles=true, position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","  West to East",	{"spawn pattern", "castleshop",erase_tiles=true, position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "castleshop",erase_tiles=true, position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a garden",
		{"spawn pattern", "castlegarden", position_relative=Vector(0,1,0)}},
	{"choice","Build an apothecary facing this direction",
		{"choice","Cancel",{"break"}},
		{"choice","  East to West",	{"spawn pattern", "castleapothecary",erase_tiles=true,position_relative=Vector(0,2,0),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "castleapothecary",erase_tiles=true,position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","  West to East",	{"spawn pattern", "castleapothecary",erase_tiles=true,position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "castleapothecary",erase_tiles=true,position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a guard barracks facing this direction",
		{"choice","Cancel",{"break"}},
		{"choice","  East to West",	{"spawn pattern", "castlebarracks",erase_tiles=true,position_relative=Vector(0,2,0),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "castlebarracks",erase_tiles=true,position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","  West to East",	{"spawn pattern", "castlebarracks",erase_tiles=true,position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "castlebarracks",erase_tiles=true,position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice", "Cancel",
		{"exit"}}},
	{"branch", check = {{"var", "built"}},
	{"func", function(q)
						--	q.object:damaged{amount = 200, type = "physical"}
					end}},
	{"loop"}}
