Dialogspec{name = "maurice",
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Maurice", "That cursed hoarder took our houses and everything."},
	{"choice", "Why are you?",
		{"say", "Maurice", "I'm Maurice. I used to be a farmer before the Hoarder cheated my house from me."},
		{"say", "Maurice", "He already lived in the biggest house to begin with but he just had to take our houses too."},
		{"branch", cond_not = "maurice known",
			{"quest", "Maurice's House", status = "active", marker = "maurice", text = ""},
			{"flag", "maurice known"}}},
	{"choice", "Why was your house taken?", cond = "maurice known",
		{"say", "Maurice", "TODO"}},
	{"choice", "What is that corpse doing here?", cond = "silverspring corpse found",
		{"say", "Maurice", "Oh, uh, that's Martin. Poor soul."},
		{"say", "Maurice", "We couldn't catch enough rats so he starved to death."},
		{"say", "Maurice", "If it wasn't for him, we all would be dead. Now we at least have something to eat."},
		{"flag", "martin known"}},
	{"choice", "Serge has asked you to leave his property.", cond = "silverspring hoarder problem known",
		{"say", "Maurice", "What, he's threatening to drive us to the caves?"},
		{"say", "Maurice", "He already got our houses? Is he never satisfied? Unbelievable!"},
		{"choice", "Perhaps I could try to persuade him to return the houses.",
			{"say", "Maurice", "We have tried already but he won't listen."},
			{"say", "Maurice", "You can try if you want but I don't think this can be settled with words."}},
		{"choice", "How much will you pay if I get you your house back?",
			{"say", "Maurice", "Pay? I have nothing! He even demanded the shirt off my back!"},
			{"say", "Maurice", "If gold is all you're about, forget about it."},
			{"say", "Maurice", "If it's not, your help is most welcome."}},
		{"choice", "I think you really should leave.",
			{"say", "Maurice", "We have gotten to this?"},
			{"say", "Maurice", "Listen, we aren't going anywhere. We have no place to go!"},
			{"say", "Maurice", "Driving us to the caves is no different than killing us."},
			{"choice", "You can only blame yourself for the situation.",
				{"say", "Maurice", "Do you really think so?"},
				{"say", "Maurice", "He ruthlessly took advantage of my weakness to get the house."},
				{"say", "Maurice", "He didn't even need it. He already had more than he could use!"},
				{"say", "Maurice", "He's hoarding everything out of greed with any means possible."},
				{"choice", "Your excuses are pathetic. Die!",
					{"func", function(q)
						q.object.spec.ai_enable_combat = true
						q.object.spec:set_factions{"evil"}
					end},
					{"exit"}},
				{"choice", "Had you stopped to think for even a moment, it all could have been avoided.",
					{"say", "Maurice", "TODO"}},
				{"choice", "I think you have a point there.",
					{"say", "Maurice", "TODO"}}}},
		{"choice", "Goodbye.",
			{"exit"}}},
	{"choice", "Goodbye.",
		{"exit"}},
	{"loop"}}

Dialogspec{name = "maurice death",
	{"quest", "Maurice's House", status = "completed", marker = "maurice", text = "Maurice has been killed."},
	{"flag", "maurice dead"}}
