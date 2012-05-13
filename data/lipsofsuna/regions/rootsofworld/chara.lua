Actorspec{
	name = "Chara",
	base = "seirei",
	dialog = "chara",
	important = true,
	marker = "chara"}

Dialogspec{name = "chara", commands = {
	{"branch", cond = "chara known",
		{"say", "Chara", "Well, well, it's one of you low-lifes again."}},
	{"branch", cond_not = "chara known",
		{"flag", "grove known"},
		{"choice", "What are you?",
			{"say", "Chara", "Low-lifes never have any kind of manners."},
			{"say", "Chara", "Talking to a lady as if she were an object, hmph."},
			{"say", "Chara", "And of all ladies, to Chara, the Seirei of the Roots of the World Tree."},
			{"branch", cond_not = "chara known",
				{"quest", "Roots of World", status = "active", text = "We have discovered Chara, the Seirei of the Roots of the World Tree."},
				{"flag", "chara known"}}},
		{"choice", "Goodbye",
			{"exit"}}},
	{"branch",
		{"choice", "Let's talk about Seirei.",
			{"choice", "What are Seirei?", cond = "chara known",
				{"say", "Chara", "Meaningless glowing things trapped into a meaningless, ever-repeating cycle of time."},
				{"choice", "No, really."},
				{"say", "Chara", "That is all. If you insist, you can call me the God, energy, faith, nature or the universe. I am all of those, yet nothing but a puppet of the cycle of time itself."}},
			{"choice", "Let's talk about something else.",
				{"break", 2}},
			{"loop"}},
		{"choice", "Let's talk about the withering of the world.",
			{"choice", "Why did the World Tree wither?", cond = "chara known",
				{"say", "Chara", "Because it made no difference anymore."},
				{"say", "Chara", "Whether the tree withers or not, the world always perishes the same way."},
				{"say", "Chara", "When the lowest of the low-lifes awakened Erinyes, it all already came to the end."},
				{"branch", cond_not = "awakening known",
					{"quest", "Roots of World", text = "Chara, the Seirei of the Roots of the World Tree, has told us that the world is about to end because a Seirei named Erinyes has been awakened."},
					{"flag", "awakening known"}}},
			{"choice", "What kind of a Seirei is Erinyes?", cond = "awakening known",
				{"say", "Chara", "Erinyes is the champion of chaos, the fundamental force of change, the bell of awakening, the violent creation and the violent destruction."},
				{"say", "Chara", "His awakening marks the ultimate and irreversible destruction as the existence is consumed by the chaos born from him awakening each and every Seirei."},
				{"branch", cond_not = "erinyes known",
					{"quest", "Roots of World", status = "active", text = "Chara, the Seirei of the Roots of the World Tree, has told us that the world is about to end because a madman of some kind awakened the Seirei named Erinyes that was sleeping within him. The awakening of Erinyes and his ability to awaken the rest of the Seirei seems to be the very reason why the world is withering."},
					{"flag", "erinyes known"}}},
			{"choice", "Tell me exactly what the culprit did to awaken Erinyes.", cond = "erinyes known",
				{"say", "Chara", "You low-lifes really enjoy this part, don't you? Oh, you do, immensely. I know it."},
				{"say", "Chara", "To get the idea, try to imagine doing all the fun stuff in the same day."},
				{"say", "Chara", "Fun stuff like lethal chemicals, drilling holes to your skull, eating your own flesh, bleeding yourself dry and more. The low-life sure had guts, at least until he rid himself of most of them."}},
			{"choice", "We will stop Erinyes. Will you help us?", cond = "erinyes known",
				{"say", "Chara", "Oh, sure I will. This is so totally new and exciting that I'm overjoyed to tears."},
				{"say", "Chara", "Except that it isn't, and I don't care even a bit."},
				{"branch", cond = "illusion opened", 
					{"say", "Chara", "Walk into the portal or walk away. It makes no difference."}},
				{"branch", cond_not = "illusion opened",
					{"say", "Chara", "You low-lifes always have a big mouth but no substance."},
					{"say", "Chara", "See for yourself what you're trying to defeat."},
					{"quest", "Roots of World", status = "completed", text = "Chara, the Seirei of the Roots of the World Tree, has told us that the world is about to end because a madman of some kind awakened the Seirei named Erinyes that was sleeping within him. The awakening of Erinyes and his ability to awaken the rest of the Seirei seems to be the very reason why the world is withering. Chara has opened a portal in her root grove to show what awaits us."},
					{"quest", "Chara's Illusion", status = "active", marker = "chara's portal", text = "Chara has opened a portal in her root grove for us. She has told us to step in to learn about defeating Erinyes."},
					{"flag", "illusion opened"}}},
			{"choice", "Let's talk about something else.",
				{"break", 2}},
			{"loop"}},
		{"choice", "Let's talk about you.", cond = "chara known",
			{"choice", "Who are you?",
				{"say", "Chara", "I am Chara, the Seirei of the Roots of the World Tree."}},
			{"choice", "What are you doing here?",
				{"say", "Chara", "Nothing meaningful, at all. This is how far I got before I stopped caring and this is how far I'll go in this era."},
				{"say", "Chara", "To make minch of the low-lifes, fly to the depths to avoid them, to stay here and listen to their blathering..."},
				{"say", "Chara", "The choice makes no difference. The outcome is always the same."}},
			{"choice", "How were you awakened and by whom?", cond = "awakening known",
				{"say", "Chara", "No big surprise there, it was one of you low-lifes, again in the right place in the right time."},
				{"say", "Chara", "You low-lifes sure know how to produce such prime individuals."},
				{"say", "Chara", "The lowest of the low always becomes the vessel of Erinyes and gets the idea to awaken him. Truly, it is marvelous how it happens every time."},
				{"say", "Chara", "Had it been any other Seirei, the low-life would have gutted himself in vain, but no, it's always Erinyes."},
				{"say", "Chara", "And because it was Erinyes, it's all of us, eventually."}},
			{"choice", "Why did you choose to awaken?", cond = "awakening known",
				{"say", "Chara", "Would you choose not to awaken if someone poured various fluids right on your face?"},
				{"choice", "Yes",
					{"say", "Chara", "That's right, in a situation like that, you don't want to stop them by showing that you're awake."},
					{"say", "Chara", "Silently, you hope that they'd pour more of their icky fluids in various places."},
					{"say", "Chara", "Truly, the low-life nature at its finest."},
					{"say", "Chara", "Low-lifes of your kind sure will be in ecstasy when death pours the fluids all over you."}},
				{"choice", "No",
					{"say", "Chara", "Right, and neither do you keep sleeping when they do that and Erinyes growls right before you in addition."}}},
			{"choice", "Let's talk about something else.",
				{"break", 2}},
			{"loop"}},
		{"choice", "Goodbye",
			{"exit"}},
		{"loop"}}}}
