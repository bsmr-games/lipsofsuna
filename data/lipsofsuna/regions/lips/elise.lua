Animationspec{
	name = "elise idle",
	animation = "aer1-tired",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "elise",
	animations = {["idle"] = "elise idle"},
	inherit = {"default"}}

Actorspec{
	name = "Elise",
	base = "aer",
	ai_type = "anchored",
	animations = {["default"] = "elise"},
	dialog = "elise",
	factions = {["neutral"] = true},
	hair_color = {200,180,20},
	hair_style = "hair2",
	important = true,
	inventory_items = {["iron hatchet"] = 1, ["dress"] = 1},
	marker = "elise",
	personality = "feeble",
	preset = "Female 2"}

Questspec{
	name = "Elise's Sorrow"}

Dialogspec{name = "elise", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch", cond = "elise informed",
		{"say", "Elise", "I want to be alone for a bit."},
		{"exit"}},
	{"say", "Elise", "Please, I need your help!"},
	{"branch",
		{"choice", "What's wrong?",
			{"say", "Elise", "Me and my friend Noemi were ambushed by monsters when we were exploring the dungeons."},
			{"say", "Elise", "I barely managed to escape but she's still there somewhere!"},
			{"say", "Elise", "Please, you need to find her!"},
			{"branch", cond_not = "ambush known",
				{"quest", "Elise's Sorrow", status = "active", marker = "elise", text = "Elise in the town of Lips has requested us to find her friend Noemi who got separated from her as they were ambushed by monsters."},
				{"flag", "ambush known"}}},
		{"choice", "Where did you get ambushed?", cond = "ambush known",
			{"say", "Elise", "It wasn't far from the town."},
			{"say", "Elise", "There was a weird grove of roots illuminated by a bright moving light."},
			{"say", "Elise", "We were going to take a look at the place but then the monsters attacked."},
			{"branch", cond_not = "grove known",
				{"quest", "Roots of World", status = "active", marker = "chara", text = "Noemi in the town of Lips has told us of a weird grove of roots illuminated by a brigh glow. Perhaps we should search the grove for clues about the fate of the World Tree."},
				{"flag", "grove known"}},
			{"branch", cond_not = "noemi known",
				{"quest", "Elise's Sorrow", status = "active", marker = "noemi", text = "Elise in the town of Lips has requested us to find her friend Noemi who got separated from her as they were ambushed by monsters. She has informed us about the place where the ambush occurred."},
				{"flag", "noemi known"}}},
		{"choice", "Why don't you try to rescue your friend yourself?", cond = "ambush known",
			{"say", "Elise", "I wish I could but I don't have the courage."},
			{"say", "Elise", "I'm no warrior, never claimed to be. They just sent me here without even telling that we have to fight!"},
			{"say", "Elise", "Just seeing those horrific monsters scares me so much I can't but run."}},
		{"choice", "I have found your friend, but...", cond = "noemi found",
			{"say", "Elise", "Sigh, I take it that she's dead."},
			{"say", "Elise", "Don't say anything, it isn't your fault."},
			{"branch", cond_not = "elise informed",
				{"say", "Elise", "You tried to help so let me teach you something that might help you in your journey."},
				{"quest", "Elise's Sorrow", status = "completed", marker = "elise", text = "We have told waiting in the town of Lips that her friend was killed by the monsters that attacked them."},
				{"unlock reward"},
				{"flag", "elise informed"}},
			{"say", "Elise", "I want to be left alone now."},
			{"exit"}},
		{"choice", "Goodbye",
			{"exit"}},
		{"loop"}}}}

Dialogspec{name = "elise death", commands = {
	{"quest", "Elise's Sorrow", status = "completed", marker = "elise", text = "Elise is dead."},
	{"flag", "elise dead"}}}
