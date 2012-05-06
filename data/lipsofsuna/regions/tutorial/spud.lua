Actorspec{
	name = "Drill Sergent Spud",
	base = "devora",
	dialog = "spud",
	marker = "spud",
	ai_type = "anchored",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["lizard dress"] = 1, ["torch"] = 1},
	personality = "tough"}

Dialogspec{name = "spud", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch",cond="training quests given",
		{"say", "Drill Sergent Spud","Got my tokens?"},
		{"choice","Yes",
			{"require player item", "combat token",
				{"require player item", "wooden dagger",
					{"branch",
						{"say","Drill Sergent Spud","Well done! Hope it wasn't too tedious. Now I can take you to Lips, where you can do largely the same things"},
						{"quest", "Down to the Dungeons", status = "active", marker = "mentor", text = "I have completed my training, and can enter deeper into the Dungeons of Suna, to an outpost known as Lips"},
						{"flag","training completed"}
					},
					{"say","Drill Sergent Spud","You don't have the Crafting token from Xsphin"}
				},
				{"say","Drill Sergent Spud","You don't have the Fighting token from Fa"}
			}
		},
		{"choice","No",
			{"say","Drill Sergent Spud","You know, I'm starting to get a bit peckish"}
		}
	},
	{"say", "Drill Sergent Spud", "If you want to learn how to craft, follow the red line to the Craftress Xsphin. Zyra the Mage is in the room next to her, go to her if you want to learn how to cast spells. To learn how to fight, follow the blue line to the Fighting Instructor Fa"},
	{"branch",cond_not="training quests given",
		{"say", "Drill Sergent Spud", "I bet you won't remember that, so let me write it in your quest log. You can check it by pressing Q, clicking on a quest will give you a handy marker on where to go"},
		{"quest", "Down to the Dungeons", status = "active", marker = "spud", text = "Spud told me where to find the numerous trainers who'll teach me how to survive in the Dungeons"},
		{"quest", "An Introduction to Crafting", status = "active", marker = "xsphin", text = "I should talk to the Craftress Xsphin to learn how to craft things"},
		{"quest", "An Introduction to Magic", status = "active", marker = "zyra", text = "I should talk to Zyra the Mage to learn how to cast spells"},
		{"quest", "An Introduction to Ultraviolence", status = "active", marker = "fa", text = "I should talk to Fa the Fighting Instructor to learn how to bash stuff"},
		{"flag","training quests given"}
	},
	{"say", "Drill Sergent Spud", "I will teleport you to the, heh, city, once you give me tokens from both of the instructors. You can leave this place without completing the quests as well, but it's a tough world out there and you'd do better to be prepared."},
	{"branch",
		{"choice", "Teleport me to town",
			{"branch",cond_not="training completed",
			{"say","Drill Sergent Spud","You haven't completed your training yet.. I'm not allowed to teleport you out of here without the tokens, but you can get out of this place if you really want to."}},
			{"branch",cond="training completed",
				{"say","Drill Sergent Spud", "Are you sure?"},
				{"branch",
					{"choice","No",
						{"say","Drill Sergent Spud","Gah, make up your mind!"}},
					{"choice","Yes, teleport away",	
						{"say", "Drill Sergent Spud", "Ok, hold on tight!"},
						{"teleport", marker = "mentor"}
					}
				}
			}
		},
		{"choice", "Goodbye.",
			{"exit"}},
		{"loop"}
	}}}
