if not Los.program_load_extension("file") then
	error("loading extension `file' failed")
end

File = Class()
File.class_name = "File"

--- Returns the contents of a directory.
-- @param self File class.
-- @param dir Directory name relative to the mod root.
-- @return Table of file names or nil.
File.scan_directory = function(self, dir)
	return Los.file_scan_directory(dir)
end

File.unittest = function()
	-- Directory scanning.
	local r1 = File:scan_directory("/")
	assert(#r1 == 1)
	assert(r1[1] == "scripts")
	local r2 = File:scan_directory("..")
	assert(r2 == nil)
	local r3 = File:scan_directory("scripts")
	assert(#r3 == 1)
	assert(r3[1] == "main.lua")
	local r4 = File:scan_directory("/scripts")
	assert(#r4 == 1)
	assert(r4[1] == "main.lua")
	local r5 = File:scan_directory("/scripts/")
	assert(#r5 == 1)
	assert(r5[1] == "main.lua")
end
