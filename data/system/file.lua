if not Los.program_load_extension("file") then
	error("loading extension `file' failed")
end

File = Class()
File.class_name = "File"

--- Reads the contents of a file.
-- @param self File class.
-- @param name File name relative to the mod root.
-- @return String or nil.
File.read = function(self, name)
	return Los.file_read(name)
end

--- Requires all the Lua files in the given directory.
-- @param self File class.
-- @param name File name relative to the mod root.
File.require_directory = function(self, name)
	for k,v in pairs(File:scan_directory(name)) do
		if string.match(v, "\.lua$") then
			require(name .. "/" .. string.gsub(v, "(.*)\.lua$", "%1"))
		end
	end
end

--- Returns the contents of a directory.
-- @param self File class.
-- @param dir Directory name relative to the mod root.
-- @return Table of file names or nil.
File.scan_directory = function(self, dir)
	return Los.file_scan_directory(dir)
end

--- Writes the contents of a file.
-- @param self File class.
-- @param name File name relative to the mod root.
-- @param data Data string.
-- @return True on success.
File.write = function(self, name, data)
	return Los.file_write(name, data)
end

File.unittest = function()
	-- Directory scanning.
	local r1 = File:scan_directory("/")
	assert(#r1 == 2)
	assert(r1[1] == "aer1-walk.lani")
	assert(r1[2] == "main.lua")
	local r2 = File:scan_directory("..")
	assert(r2 == nil)
	local r3 = File:scan_directory("")
	assert(#r3 == 2)
	assert(r3[2] == "main.lua")
	local r4 = File:scan_directory(".")
	assert(#r4 == 2)
	assert(r4[2] == "main.lua")
	local r5 = File:scan_directory("./")
	assert(#r5 == 2)
	assert(r5[2] == "main.lua")
end
