Actorspec{
	name = "Expedition Leader Lyra",
	base = "aer",
	dialog = "lyra",
	marker = "lyra",
	ai_type = "anchored",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["dress"] = 1},
	personality = "feeble",
	preset = "Female 2"}

Dialogspec{name = "lyra", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch", cond_not = "lyra introduced",
		{"say", "Expedition Leader Lyra", "Step up, let me have a look at you.."},
		{"say", "Expedition Leader Lyra", "You'll do well..."}, --some conditional dialoge here, based on the race of the player
		{"say", "Expedition Leader Lyra", "Anyhow, welcome to the caverns of Suna, I'm sure you'll enjoy the wonderful sights and experiences it has to offer."},
		{"say", "Expedition Leader Lyra", "If, of course, you enjoy having your limbs dismembered and splattered in front of you."},
		{"flag","lyra introduced"},
		{"notification", "Use Y to toggle the first person camera mode."},
		{"say", "Expedition Leader Lyra", "Damnit, that torch is about to go out, I can see it flickering. Go fetch it, trust me, this is not a pleasant place to be stuck in the dark"},
		{"quest", "Down to the Dungeons", status = "active", marker = "dying torch", text = "Lyra told me to go fetch a dying torch"}
	},
	{"branch", cond_not = "torch fetched",
		{"say", "Expedition Leader Lyra", "Where's my torch?"},
		{"branch",
			{"choice", "I can't reach it, it's too high",
				{"notification", "Jump with SPACE and climb with C."},
				{"say", "Expedition Leader Lyra", "What do you mean, it's too high? Just jump and climb up the ledge. Now fetch it, before I throw you down this damned shaft"},
				{"exit"}
			},
			{"choice", "Here it is.",
				{"require player item", "dying torch",
					{"branch",
						{"say", "Expedition Leader Lyra", "There we go, let me re-light it"},
						{"say", "Expedition Leader Lyra", "Anyhow, now that that's done, let me record you into the Book of Adventurers, and then we can start your training"},
						{"say", "Expedition Leader Lyra", "Ok, done. You'll have to clamber down the shaft and report to Drill Sergent Spud. Xe'll give you further instructions. Don't fall and break your neck, we spend enough time cleaning up pieces of adventurers as is."},
						{"flag", "torch fetched"},
						{"quest", "Down to the Dungeons", status = "active",marker="spud", text = "Lyra told me to report to Drill Sergent Spud for further instructions"},
						{"unlock reward"}
					},
					{"branch",
						{"say", "Expedition Leader Lyra", "Don't try to fool me. I can see you don't have it. Now go fetch it!"},
						{"exit"}
					}
				}
			}
		}
	},
	{"say","Expedition Leader Lyra","I'd love to stay and chat, but the next freight of monster meat is arriving in a short while, and I have to reinforce my reserves of patience"},
	{"choice", "Goodbye.",
		{"exit"}},
	{"loop"}}}
