Obstaclespec{
	name = "portal of midguard",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "portal of midguard",
	marker = "portal of midguard",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "portal of midguard",
	commands = {
		{"branch", cond = "portal of midguard activated",
			{"branch", cond = "portal of lips activated",
				{"teleport", marker = "portal of lips"},
				{"effect player", "portal1"},
				{"exit"}
			},
			{"info", "The other end of the portal is closed."},
			{"exit"}
		},
		{"quest", "Portal of Midguard", status = "completed", marker = "portal of midguard", text = "The portal of Midguard has been activated."},
		{"flag", "portal of midguard activated"},
		{"info", "You have activated the portal of Midguard."}
	}}
