Skillspec{
	name = "Health lv1",
	description = "Increase your base health to 40 points.",
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Health lv2",
	description = "Increase your base health to 60 points.",
	requires = {"Health lv1"},
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Health lv3",
	description = "Increase your base health to 80 points.",
	requires = {"Health lv2"},
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Health lv4",
	description = "Increase your base health to 100 points.",
	requires = {"Health lv3"},
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Willpower lv1",
	description = "Increase your base willpower to 40 points.",
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Willpower lv2",
	description = "Increase your base willpower to 60 points.",
	requires = {"Willpower lv1"},
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Willpower lv3",
	description = "Increase your base willpower to 80 points.",
	requires = {"Willpower lv2"},
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Willpower lv4",
	description = "Increase your base willpower to 100 points.",
	requires = {"Willpower lv3"},
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Sprinter",
	description = "Run and walk twice as fast.",
	assign = function(skills) skills.speed = skills.speed * 2 end}

Skillspec{
	name = "Far sight",
	description = "Spot items and creatures twice as far.",
	assign = function(skills) skills.view_distance = skills.view_distance * 2 end}

Skillspec{
	name = "Fighter",
	description = "Melee attacks do double damage.",
	assign = function(skills) skills.melee = skills.melee * 2 end}

Skillspec{
	name = "Marksman",
	description = "Ranged weapons do double damage.",
	assign = function(skills) skills.ranged = skills.ranged * 2 end}
