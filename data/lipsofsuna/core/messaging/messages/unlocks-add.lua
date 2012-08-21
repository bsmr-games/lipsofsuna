Message{
	name = "unlocks add",
	server_to_client_encode = function(self, type, name)
		return {"string", type, "string", name}
	end,
	server_to_client_decode = function(self, packet)
		local ok,type,name = packet:read("string", "string")
		if not ok then return end
		return {type, name}
	end,
	server_to_client_handle = function(self, type, name)
		Client.data.unlocks:unlock(type, name)
		if not Operators.play:is_startup_period() then
			Client:append_log("Unlocked " .. type .. ": " .. name)
		end
	end}
