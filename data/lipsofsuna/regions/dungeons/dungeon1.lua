Staticspec{
	name = "dungeon entrance(1)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(1)",
	marker = "dungeon entrance(1)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(1)",
	commands = {
		{"teleport", marker = "dungeon exit(1)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(1)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(1)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(1)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(1)",
	marker = "dungeon exit(1)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(1)",
	commands = {
		{"teleport", marker = "dungeon entrance(1)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(1)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(1)"}}}
