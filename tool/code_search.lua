#! /usr/bin/lua

require "lfs"
local Class = require("../data/system/class")

------------------------------------------------------------------------------

local Utils = Class("Utils")

Utils.get_file_type = function(self, name)
	if string.match(name, ".*[.]c$") then return "C" end
	if string.match(name, ".*[.]cpp$") then return "C++" end
	if string.match(name, ".*[.]cs$") then return "C#" end
	if string.match(name, ".*[.]h$") then return "C" end
	if string.match(name, ".*[.]hpp$") then return "C++" end
	if string.match(name, ".*[.]lua") then return "Lua" end
	if string.match(name, ".*[.]php") then return "PHP" end
	if string.match(name, ".*[.]py$") then return "Python" end
	if string.match(name, ".*[.]vhd$") then return "VHDL" end
end

Utils.find_files = function(self, path)
	return coroutine.wrap(function()
		local recurse
		recurse = function(path)
			for name in lfs.dir(path) do
				if not string.match(name, "^[.]") then
					local file = path .. name
					local attr = lfs.attributes(file)
					if attr.mode == "directory" then
						recurse(file .. "/")
					else
						coroutine.yield(path, name)
					end
				end
			end
		end
		recurse(path .. "/")
	end)
end

Utils.usage = function(self)
	print([[

code_search.lua [OPTIONS] [PATH]

Options:
  -h  --help                      Show this help message and exit.
  -l  --lines                     Count lines of code.
  -r  --replace STR STR           Replace a string.
  -rp --replace-pattern STR STR   Replace a pattern.
  -rw --replace-whole STR STR     Replace a pattern in the whole document.
  -s  --search STR                Search for a string.
  -sp --search-pattern STR        Search for a pattern.
]])
end

------------------------------------------------------------------------------

local Parse = Class("Parse")

Parse.new = function(clss, path, name)
	local self = Class.new(clss)
	self.path = path
	self.name = name
	self.number = 0
	self.stack = {}
	return self
end

Parse.parse = function(self)
	local file = self:start()
	if not file then return end
	for l in file:lines() do
		self.number = self.number + 1
		self:line(l)
	end
	self:done()
	file:close()
end

Parse.start = function(self)
	if not Utils:get_file_type(self.name) then return end
	return io.open(self.path .. self.name, "r")
end

Parse.line = function(self, line)
end

Parse.done = function(self)
end

------------------------------------------------------------------------------

local Match = Class("Match", Parse)

Match.new = function(clss, path, name)
	local self = Parse.new(clss, path, name)
	self.found = false
	return self
end

Match.line = function(self, line)
	if self:match(line) then
		if not self.found then
			self.found = true
			print("")
			print("In file " .. self.path .. self.name .. " :")
		end
		print(string.format("%d: %s", self.number, line))
	end
end

Match.match = function(self, line)
end

------------------------------------------------------------------------------

local MatchPattern = Class("MatchPattern", Match)

MatchPattern.new = function(clss, path, name, expr)
	local self = Match.new(clss, path, name)
	self.expression = expr
	return self
end

MatchPattern.match = function(self, line)
	return string.find(line, self.expression)
end

------------------------------------------------------------------------------

local MatchString = Class("MatchString", Match)

MatchString.new = function(clss, path, name, expr)
	local self = Match.new(clss, path, name)
	self.expression = expr
	return self
end

MatchString.match = function(self, line)
	return string.find(line, self.expression, 1, true)
end

------------------------------------------------------------------------------

local MatchWhole = Class("MatchWhole", Match)

MatchWhole.new = function(clss, path, name, expr)
	local self = Match.new(clss, path, name)
	self.expression = expr
	return self
end

MatchWhole.parse = function(self)
	local file = self:start()
	if not file then return end
	local text = file:read("*a")
	self:match(text)
	self:done()
	file:close()
end

MatchWhole.match = function(self, line)
	for m in string.gmatch(line, self.expression) do
		if not self.found then
			self.found = true
			print("")
			print("In file " .. self.path .. self.name .. " :")
		end
		print(m)
	end
end

------------------------------------------------------------------------------

local Replace = Class("Replace", Match)

Replace.new = function(clss, path, name, expr, repl)
	local self = Match.new(clss, path, name)
	self.count = 0
	self.output = {}
	return self
end

Replace.line = function(self, line)
	local res = self:replace(line)
	if res then
		table.insert(self.output, res)
		self.count = self.count + 1
	else
		table.insert(self.output, line)
	end
end

Replace.replace = function(self, line)
end

Replace.done = function(self)
	if self.count == 0 then return end
	print("In file " .. self.path .. self.name .. " : " .. self.count)
	table.insert(self.output, "")
	local res = table.concat(self.output, "\n")
	local file = io.open(self.path .. self.name, "w")
	file:write(res)
	file:close()
end

------------------------------------------------------------------------------

local ReplaceString = Class("ReplaceString", Replace)

ReplaceString.new = function(clss, path, name, expr, repl)
	local self = Replace.new(clss, path, name)
	self.expression = expr
	self.replacement = repl
	return self
end

ReplaceString.replace = function(self, line)
	local s,e = string.find(line, self.expression, 1, true)
	if not s then return end
	return string.sub(line, 1, s - 1) .. self.replacement .. string.sub(line, e + 1)
end

------------------------------------------------------------------------------

local ReplacePattern = Class("ReplacePattern", Replace)

ReplacePattern.new = function(clss, path, name, expr, repl)
	local self = Replace.new(clss, path, name)
	self.expression = expr
	self.replacement = repl
	return self
end

ReplacePattern.replace = function(self, line)
	local res,num = string.gsub(line, self.expression, self.replacement)
	if num == 0 then return end
	return res
end

------------------------------------------------------------------------------

local ReplaceWhole = Class("ReplaceWhole", ReplacePattern)

ReplaceWhole.new = function(clss, path, name, expr, repl)
	local self = ReplacePattern.new(clss, path, name, expr, repl)
	return self
end

ReplaceWhole.parse = function(self)
	local file = self:start()
	if not file then return end
	local text = file:read("*a")
	self:line(text)
	self:done()
	file:close()
end

------------------------------------------------------------------------------

local ReplaceXXX = Class("ReplaceXXX", ReplaceWhole)

ReplaceXXX.new = function(clss, path, name)
	local self = Replace.new(clss, path, name)
	return self
end

ReplaceXXX.parse = function(self)
	local file = self:start()
	if not file then return end
	local text = file:read("*a")
	self:line(text)
	self:done()
	file:close()
end

ReplaceXXX.replace = function(self, line)
	if string.find(line, "--@module ") then return end
	local s1,e1 = string.find(line, "\nlocal ([a-zA-Z0-9]+) = Class%([^)]*%)")
	local s2,e2 = string.find(line, "\n([a-zA-Z0-9]+) = Class%([^)]*%)")
	local s,e = s1 or s2,e1 or e2
	if not s then return end
	local name = string.match(line, "([.a-zA-Z0-9]+) = Class%([^)]*%)")
	if not name then return end

	local x = string.gsub(self.path .. self.name, "^data/", "")
	local x = string.gsub(x, "%.lua$", "")
	local x = string.gsub(x, "/", ".")
	local x = string.gsub(x, "-", "_")
	return string.format(
[[--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module %s
-- @alias %s

%s
--- TODO:doc
-- @type %s%s
]], x, name, string.sub(line, 1, s - 1), name, string.sub(line, s))
end

------------------------------------------------------------------------------

local CountLines = Class("CountLines")

CountLines.new = function(clss)
	local self = Class.new(clss)
	self.langs = {}
	return self
end

CountLines.parse = function(self, path, name)
	local lang = Utils:get_file_type(name)
	if not lang then return end
	local num = self.langs[lang] or 0
	local file = io.open(path .. name, "r")
	for l in file:lines() do
		num = num + 1
	end
	file:close()
	self.langs[lang] = num
end

CountLines.get_total = function(self)
	local total = 0
	for k,v in pairs(self.langs) do
		total = total + v
	end
	return total
end

CountLines.get_list = function(self)
	local list = {}
	for k,v in pairs(self.langs) do
		table.insert(list, {k, v})
	end
	table.sort(list, function(a, b) return a[2] > b[2] end)
	return list
end

------------------------------------------------------------------------------

if #arg == 0 or arg[1] == "-h" or arg[1] == "--help" then
	return Utils:usage()
elseif arg[1] == "-l" or arg[1] == "--lines" then
	if #arg ~= 1 and #arg ~= 2 then return Utils:usage() end
	local count = CountLines()
	for path,name in Utils:find_files(arg[2] or ".") do
		count:parse(path, name)
	end
	for k,v in pairs(count:get_list()) do
		print(string.format("%6s: %d", v[1], v[2]))
	end
	print(string.format("\n%6s: %d", "Total", count:get_total()))
elseif arg[1] == "-r" or arg[1] == "--replace" then
	if #arg ~= 3 and #arg ~= 4 then return Utils:usage() end
	for path,name in Utils:find_files(arg[4] or ".") do
		ReplaceString(path, name, arg[2], arg[3]):parse()
	end
elseif arg[1] == "-rp" or arg[1] == "--replace-pattern" then
	if #arg ~= 3 and #arg ~= 4 then return Utils:usage() end
	for path,name in Utils:find_files(arg[4] or ".") do
		ReplacePattern(path, name, arg[2], arg[3]):parse()
	end
elseif arg[1] == "-rw" or arg[1] == "--replace-whole" then
	if #arg ~= 3 and #arg ~= 4 then return Utils:usage() end
	for path,name in Utils:find_files(arg[4] or ".") do
		ReplaceWhole(path, name, arg[2], arg[3]):parse()
	end
elseif arg[1] == "-s" or arg[1] == "--search" then
	if #arg ~= 2 and #arg ~= 3 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchString(path, name, arg[2]):parse()
	end
elseif arg[1] == "-sp" or arg[1] == "--search-pattern" then
	if #arg ~= 2 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchPattern(path, name, arg[2]):parse()
	end
elseif arg[1] == "-sw" or arg[1] == "--search-whole" then
	if #arg ~= 2 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchWhole(path, name, arg[2]):parse()
	end
elseif arg[1] == "-X" then
	for path,name in Utils:find_files(arg[2]) do
		ReplaceXXX(path, name):parse()
	end
else
	return Utils:usage()
end