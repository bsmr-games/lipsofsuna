Staticspec{
	name = "dungeon entrance(4)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(4)",
	marker = "dungeon entrance(4)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(4)",
	commands = {
		{"teleport", marker = "dungeon exit(4)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(4)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(4)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(4)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(4)",
	marker = "dungeon exit(4)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(4)",
	commands = {
		{"teleport", marker = "dungeon entrance(4)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(4)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(4)"}}}