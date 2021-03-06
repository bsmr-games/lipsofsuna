-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "change privilege level",
	server_to_client_encode = function(self, value)
		return {"bool", value}
	end,
	server_to_client_decode = function(self, packet)
		local ok,value = packet:read("bool")
		if not ok then return end
		return {value}
	end,
	server_to_client_handle = function(self, value)
		Client.data.admin.enabled = value
	end}
