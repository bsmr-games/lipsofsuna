local Class = require("system/class")

if not Los.program_load_extension("network") then
	error("loading extension `network' failed")
end

------------------------------------------------------------------------------

local Packet = Class("Packet")

--- Creates a new packet.
-- @param self Packet class.
-- @param ... Packet type and contents.
-- @return New packet.
Packet.new = function(clss, ...)
	local self = Class.new(clss)
	self.handle = Los.packet_new(...)
	return self
end

--- Creates a new packet from an internal handle.
-- @param self Packet class.
-- @param handle Handle.
-- @return New packet.
Packet.new_from_handle = function(clss, handle)
	local self = Class.new(clss)
	self.handle = handle
	return self
end

--- Reads data starting from the beginning of the packet.
-- @param self Packet.
-- @param ... Types to read.
-- @return Boolean and a list of read values.
Packet.read = function(self, ...)
	return Los.packet_read(self.handle, ...)
end

--- Reads data starting from the beginning of the packet.
-- @param self Packet.
-- @param ... Types to read.
-- @return Boolean and a table of read values.
Packet.read_table = function(self, ...)
	local pack = function(...) return {...} end
	local res = pack(self:read(...))
	local ok = res[1]
	table.remove(res, 1)
	return ok,res
end

--- Reads data starting from the last read positiong of the packet.
-- @param self Packet.
-- @param ... Types to read.
-- @return Boolean and a list of read values.
Packet.resume = function(self, ...)
	return Los.packet_resume(self.handle, ...)
end

--- Reads data starting from the beginning of the packet.
-- @param self Packet.
-- @param ... Types to read.
-- @return Boolean and a table of read values.
Packet.resume_table = function(self, ...)
	local pack = function(...) return {...} end
	local res = pack(self:resume(...))
	local ok = res[1]
	table.remove(res, 1)
	return ok,res
end

--- Appends data to the packet.
-- @param self Packet.
-- @param ... Types to write.
Packet.write = function(self, ...)
	return Los.packet_write(self.handle, ...)
end

--- Gets the size of the packet, in bytes.
-- @param self Packet.
-- @return Size in bytes.
Packet.get_size = function(self)
	return Los.packet_get_size(self.handle)
end

--- Gets the type number of the packet.
-- @param self Packet.
-- @return Number.
Packet.get_type = function(self)
	return Los.packet_get_type(self.handle)
end

--- Sets the type number of the packet.
-- @param self Packet.
-- @param v Number.
Packet.set_type = function(self, v)
	Los.packet_set_type(self.handle, v)
end

return Packet
