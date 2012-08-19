Message{
	name = "add inventory item",
	server_to_client_encode = function(self, id, index, name, count)
		return {"uint32", id, "uint8", index, "string", name, "uint32", count}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,index,name,count = packet:read("uint32", "uint8", "string", "uint32")
		if not ok then return end
		return {id, index, name,count}
	end,
	server_to_client_handle = function(self, id, index, name, count)
		-- Find the object.
		local object = Object:find{id = id}
		if not object then return end
		-- Add to the inventory.
		if not object:has_server_data() then
			local spec = Itemspec:find{name = name}
			if not spec then return end
			object.inventory:set_object(index, Item{spec = spec, count = count})
		end
		-- Update the user interface.
		if Ui.state == "crafting" and object == Client.player_object then
			Operators.crafting:update_craftability()
		elseif Ui.state == "inventory" and object == Client.player_object then
			Ui:restart_state()
		elseif Ui.state == "loot" and object.id == Client.data.inventory.id then
			Ui:restart_state()
		elseif Ui.state == "store" and object == Client.player_object then
			Ui:restart_state()
		end
	end}