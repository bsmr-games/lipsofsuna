Actionspec{
	name = "examine",
	label = "Examine",
	func = function(self, user)
		if Server.dialogs:execute(self, user) then
			Main.messaging:server_event("object dialog", user.client, self:get_id())
		end
	end}
