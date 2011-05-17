require "client/action"
require "client/camera"
require "client/shader"
require "client/target"
require "client/theme"
require "client/shaders/adamantium"
require "client/shaders/default"
require "client/shaders/diffnormspec"
require "client/shaders/eye"
require "client/shaders/glass"
require "client/shaders/foliage"
require "client/shaders/fur"
require "client/shaders/hair"
require "client/shaders/lava"
require "client/shaders/luminous"
require "client/shaders/normalmap"
require "client/shaders/nolitdiff"
require "client/shaders/partblend"
require "client/shaders/particle"
require "client/shaders/postprocess"
require "client/shaders/skin"
require "client/shaders/speedline"
require "client/shaders/terrain"
require "client/shaders/water"
require "client/shaders/widget"
require "client/widgets/background"
require "client/widgets/scene"
require "editor/controls"
require "editor/events"
require "editor/selection"

Object:add_getters{
	spec = function(s) return rawget(s, "__spec") end}

Object:add_setters{
	spec = function(s, v)
		rawset(s, "__spec", v)
		s.model = v.model
		if v.models then
			local m = s.model:copy()
			for k,v in pairs(v.models) do
				if k ~= "skeleton" then
					m:merge(Model:load{file = v})
				end
			end
			m:calculate_bounds()
			m:changed()
			s.model = m
		end
		if v.type == "species" then
			s:animate{animation = "idle", channel = 1, permanent = true}
			s:update_animations{secs = 0}
			s:deform_mesh()
		end
	end}

Object.move = function(self, value, step)
	self.position = self.position + value * step
end

Object.snap = function(self, step)
	local v = self.position
	v.x = v.x - v.x % step
	v.y = v.y - v.y % step
	v.z = v.z - v.z % step
	self.position = v
end

--- Rotates the object with step snapping.
-- @param self Object.
-- @param value Step size.
-- @param steps Number of steps per revolution.
Object.rotate = function(self, value, steps)
	local s = 2 * math.pi / steps
	local e = self.rotation.euler
	e[1] = math.floor(e[1] / s + value + 0.5) * s
	self.rotation = Quaternion{euler = e}
end

for k,s in pairs(Shader.dict_name) do
	s:set_quality(2)
end

Editor = Class()

Editor.new = function(clss)
	-- Find or create the pattern.
	local pattern = Pattern:find{name = Settings.pattern}
	if not pattern then
		pattern = Pattern{name = Settings.pattern, size = Vector(32,8,32)}
		local i = 1
		for x=0,31 do
			for z=0,31 do
				pattern.tiles[i] = {x,0,z,"granite1"}
				i = i + 1
			end
		end
	end
	-- Initialize self.
	local self = Class.new(clss)
	self.prev_tiles = {}
	self.mouse_sensitivity = 0.01
	self.selection = {}
	-- Load the pattern.
	self.pattern = pattern
	self.origin = Vector(100,100,100)
	self.size = pattern.size or Vector(10,10,10)
	Voxel:place_pattern{point = self.origin, name = Settings.pattern}
	-- Camera and lighting.
	self.camera = FirstPersonCamera()
	self.camera:warp((self.origin + self.size * 0.5) * Voxel.tile_size, Quaternion(0, 1, 0, 0))
	self.light = Light{ambient = {0.3,0.3,0.3,1.0}, diffuse={0.6,0.6,0.6,1.0}, equation={1.0,0.0,0.01}, priority = 2}
	self.light.enabled = true
	-- Item selector.
	local items = {}
	for k in pairs(Itemspec.dict_name) do table.insert(items, k) end
	table.sort(items)
	self.combo_items = Widgets.ComboBox(items)
	self.combo_items:activate{index = 1, pressed = false}
	self.button_items = Widgets.Button{text = "Add", pressed = function() self.mode = "add item" end}
	self.group_items = Widget{cols = 2, rows = 1}
	self.group_items:set_expand{col = 1}
	self.group_items:set_child{col = 1, row = 1, widget = self.combo_items}
	self.group_items:set_child{col = 2, row = 1, widget = self.button_items}
	-- Obstacle selector.
	local obstacles = {}
	for k in pairs(Obstaclespec.dict_name) do table.insert(obstacles, k) end
	table.sort(obstacles)
	self.combo_obstacles = Widgets.ComboBox(obstacles)
	self.combo_obstacles:activate{index = 1, pressed = false}
	self.button_obstacles = Widgets.Button{text = "Add", pressed = function() self.mode = "add obstacle" end}
	self.group_obstacles = Widget{cols = 2, rows = 1}
	self.group_obstacles:set_expand{col = 1}
	self.group_obstacles:set_child{col = 1, row = 1, widget = self.combo_obstacles}
	self.group_obstacles:set_child{col = 2, row = 1, widget = self.button_obstacles}
	-- Species selector.
	local species = {}
	for k in pairs(Species.dict_name) do table.insert(species, k) end
	table.sort(species)
	self.combo_species = Widgets.ComboBox(species)
	self.combo_species:activate{index = 1, pressed = false}
	self.button_species = Widgets.Button{text = "Add", pressed = function() self.mode = "add species" end}
	self.group_species = Widget{cols = 2, rows = 1}
	self.group_species:set_expand{col = 1}
	self.group_species:set_child{col = 1, row = 1, widget = self.combo_species}
	self.group_species:set_child{col = 2, row = 1, widget = self.button_species}
	-- Tile selector.
	local tiles = {}
	for k in pairs(Material.dict_name) do table.insert(tiles, k) end
	table.sort(tiles)
	self.combo_tiles = Widgets.ComboBox(tiles)
	self.combo_tiles:activate{index = 1, pressed = false}
	self.button_tiles = Widgets.Button{text = "Add", pressed = function() self.mode = "add tile" end}
	self.group_tiles = Widget{cols = 2, rows = 1}
	self.group_tiles:set_expand{col = 1}
	self.group_tiles:set_child{col = 1, row = 1, widget = self.combo_tiles}
	self.group_tiles:set_child{col = 2, row = 1, widget = self.button_tiles}
	-- Buttons.
	self.button_delete = Widgets.Button{text = "Delete", pressed = function() self.mode = "delete" end}
	self.button_save = Widgets.Button{text = "Save", pressed = function() self:save() end}
	-- Packing.
	self.group = Widgets.Frame{cols = 1, rows = 6}
	self.group:set_expand{col = 1}
	self.group:set_child{col = 1, row = 1, widget = self.group_items}
	self.group:set_child{col = 1, row = 2, widget = self.group_obstacles}
	self.group:set_child{col = 1, row = 3, widget = self.group_species}
	self.group:set_child{col = 1, row = 4, widget = self.group_tiles}
	self.group:set_child{col = 1, row = 5, widget = self.button_delete}
	self.group:set_child{col = 1, row = 6, widget = self.button_save}
	self.scene = Widgets.Scene{cols = 2, rows = 2, camera = self.camera, margins = {5,5,0,0}, behind = true, floating = true, fullscreen = true}
	self.scene.pressed = function(w, args) self:pressed(args) end
	self.scene:set_expand{col = 2, row = 2}
	self.scene:set_child{col = 1, row = 1, widget = self.group}
	-- Corner markers.
	self.corners = Object{position = self.origin * Voxel.tile_size, realized = true}
	self.corners.model = Model()
	self.corners.model:add_material{cull = false, shader = "default"}
	self:update_corners()
	return self
end

--- Extrudes the selected tiles.
-- @param self Editor.
-- @param rev True to extrude in reverse.
Editor.extrude = function(self, reverse)
	local del = {}
	local add = {}
	for k,v in pairs(self.selection) do
		if v.tile then
			if reverse then
				-- Removal.
				local d = Selection.face_dir[v.face]
				local p = v.tile - d
				Voxel:set_tile(v.tile, 0)
				table.insert(del, v)
				if Voxel:get_tile(p) ~= 0 then
					table.insert(add, Selection(p, v.face))
				end
			else
				-- Creation.
				local d = Selection.face_dir[v.face]
				local p = v.tile + d
				Voxel:set_tile(p, Voxel:get_tile(v.tile))
				table.insert(del, v)
				if Voxel:get_tile(p + d) == 0 then
					table.insert(add, Selection(p, v.face))
				end
			end
		end
	end
	for k,v in pairs(del) do
		self.selection[v.key] = nil
		v:detach()
	end
	for k,v in pairs(add) do
		local s = self.selection[v.key]
		if s then s:detach() end
		self.selection[v.key] = v
	end
	self.selection_prev = nil
	self:update_rect_select()
end

Editor.fill = function(self, p1, p2, erase)
	-- Find the fill material.
	local mat = 0
	if not erase then
		local m = Material:find{name = self.combo_tiles.text}
		if m then mat = m.id end
	end
	-- Fix the order of the range.
	local x1,x2 = p1.x,p2.x
	local y1,y2 = p1.y,p2.y
	local z1,z2 = p1.z,p2.z
	if x1 > x2 then x1,x2 = x2,x1 end
	if y1 > y2 then y1,y2 = y2,y1 end
	if z1 > z2 then z1,z2 = z2,z1 end
	-- Fill all voxels within the range.
	for x = x1,x2 do
		for y = y1,y2 do
			for z = z1,z2 do
				Voxel:set_tile(Vector(x,y,z), mat)
			end
		end
	end
end

Editor.pressed = function(self, args)
	local point,object,tile = Target:pick_ray{camera = self.camera}
	if not point then return end
	if args.button ~= 1 then return end
	if self.mode == "add item" then
		local spec = Itemspec:find{name = self.combo_items.text}
		Object{position = point, realized = true, spec = spec}
	elseif self.mode == "add obstacle" then
		local spec = Obstaclespec:find{name = self.combo_obstacles.text}
		Object{position = point, realized = true, spec = spec}
	elseif self.mode == "add species" then
		local spec = Species:find{name = self.combo_species.text}
		Object{position = point, realized = true, spec = spec}
	elseif self.mode == "add tile" then
		local mat = Material:find{name = self.combo_tiles.text}
		local t,p = Voxel:find_tile{match = "empty", point = point}
		if p then
			Voxel:set_tile(p, mat.id)
			self.prev_tiles[2] = self.prev_tiles[1]
			self.prev_tiles[1] = p
		end
	elseif self.mode == "delete" then
		if object then
			if object.spec then
				object.realized = false
			end
		elseif tile then
			Voxel:set_tile(tile, 0)
		end
	end
end

Editor.save = function(self)
	local items = {}
	local obstacles = {}
	local species = {}
	-- Collect objects.
	for k,v in pairs(Object.objects) do
		if v.realized and v.spec then
			if v.spec.type == "item" then
				table.insert(items, v)
			elseif v.spec.type == "obstacle" then
				table.insert(obstacles, v)
			elseif v.spec.type == "species" then
				table.insert(species, v)
			end
		end
	end
	-- Sort objects.
	-- Predictable order leads to cleaner patches.
	local roundvec = function(v)
		local p = v * Voxel.tile_scale - self.origin
		return {math.floor(p.x * 100 + 0.5) * 0.01,
		        math.floor(p.y * 100 + 0.5) * 0.01,
		        math.floor(p.z * 100 + 0.5) * 0.01}
	end
	local sortobj = function(a, b)
		local ap = roundvec(a.position)
		local bp = roundvec(b.position)
		if ap[1] < bp[1] then return true end
		if ap[1] > bp[1] then return false end
		if ap[2] < bp[2] then return true end
		if ap[2] > bp[2] then return false end
		if ap[3] < bp[3] then return true end
		if ap[3] > bp[3] then return false end
		if a.spec.name < a.spec.name then return true end
		return false
	end
	table.sort(items, sortobj)
	table.sort(obstacles, sortobj)
	table.sort(species, sortobj)
	-- Format categories.
	local t = "Pattern{\n\tname = \"" .. self.pattern.name .. "\",\n\tsize = " .. tostring(self.size) .. ",\n"
	if self.pattern.categories then
		local categories = {}
		for k,v in pairs(self.pattern.categories) do
			table.insert(categories, k)
		end
		table.sort(categories)
		t = t .. "\tcategories = {"
		local comma
		for k,v in ipairs(categories) do
			if comma then t = t .. "," end
			t = t .. "\"" .. v .. "\""
			comma = true
		end
		t = t .. "},\n"
	end
	-- Format objects.
	local addobj = function(t, k, v)
		local p = roundvec(v.position)
		local r = v.rotation.euler
		if r[1] ~= 0 then
			local e = math.floor(100 * r[1] / (2 * math.pi) + 0.5) / 100
			if e == -0.5 then e = 0.5 end
			return string.format("%s%s\t\t{%f,%f,%f,%q,%.2f}",
				t, k > 1 and ",\n" or "", p[1], p[2], p[3], v.spec.name, e)
		else
			return string.format("%s%s\t\t{%f,%f,%f,%q}",
				t, k > 1 and ",\n" or "", p[1], p[2], p[3], v.spec.name)
		end
	end
	local comma
	if #items > 0 then
		t = t .. "\titems = {\n"
		for k,v in ipairs(items) do t = addobj(t, k, v) end
		t = t .. "}"
		comma = true
	end
	if #obstacles > 0 then
		if comma then t = t .. ",\n" end
		t = t .. "\tobstacles = {\n"
		for k,v in ipairs(obstacles) do t = addobj(t, k, v) end
		t = t .. "}"
		comma = true
	end
	if #species > 0 then
		if comma then t = t .. ",\n" end
		t = t .. "\tcreatures = {\n"
		for k,v in ipairs(species) do t = addobj(t, k, v) end
		t = t .. "}"
		comma = true
	end
	-- Format tiles.
	local newline
	local first = true
	for z = 0,self.size.z-1 do
		for y = 0,self.size.y-1 do
			newline = true
			for x = 0,self.size.x-1 do
				local v = Voxel:get_tile(self.origin + Vector(x, y, z))
				if v ~= 0 then
					local mat = Material:find{id = v}
					if mat then
						if first then
							if comma then t = t .. ",\n" end
							t = t .. "\ttiles = {\n\t\t"
							first = nil
							newline = nil
						elseif newline then
							t = t .. ",\n\t\t"
							newline = nil
						else
							t = t .. ", "
						end
						t = t .. "{" .. x .. "," .. y .. "," .. z .. ",\"" .. mat.name .. "\"}"
					end
				end
			end
		end
	end
	if not first then
		t = t .. "}"
	end
	t = t .. "}\n"
	-- Write to the file.
	local name = string.format("scripts/content/patterns/%s.lua", self.pattern.name)
	if not File:write(name, t) then
		print(string.format("ERROR: Could not save `%s'.", name))
	end
end

Editor.pick = function(self)
	-- Pick from the scene.
	local hl = self.highlight and self.highlight.visual
	local mode = Client.video_mode
	local r1,r2 = self.camera:picking_ray{cursor = Vector(mode[1]/2, mode[2]/2), far = 20, near = 0.1}
	local ret = Physics:cast_ray{src = r1, dst = r2}
	if not ret then return end
	local point,object,tile = ret.point, ret.object, ret.tile
	-- Find or create a selection.
	if object then
		if object.selection then return object.selection end
		local i = object
		local s = self.selection[i]
		if s then return s end
		return Selection(object)
	elseif tile then
		-- Determine the face.
		local r = point * Voxel.tile_scale - (tile + Vector(0.5,0.5,0.5))
		local rx = math.abs(r.x)
		local ry = math.abs(r.y)
		local rz = math.abs(r.z)
		local face
		if rx > ry and rx > rz then
			face = r.x < 0 and 1 or 2
		elseif ry > rz then
			face = r.y < 0 and 3 or 4
		else
			face = r.z < 0 and 5 or 6
		end
		-- Find or create the tile.
		local i = Selection:get_tile_key(tile, face)
		local s = self.selection[i]
		if s then return s end
		return Selection(tile, face)
	end
end

--- Toggles the selection of the highlighted tile or object.
-- @param self Editor.
-- @param add True to add to the selection instead of replacing it.
Editor.select = function(self, add)
	local h = self.highlight
	if not h then return end
	if self.selection[h.key] then
		-- Unselect.
		self.selection[h.key] = nil
		self.selection_prev = nil
		self:update_rect_select()
	else
		-- Deselect.
		if not add and not self.selection_rect then
			for k,v in pairs(self.selection) do
				v:detach()
				self.selection[k] = nil
			end
		end
		-- Select.
		self.selection[h.key] = h
		self.selection_prev = h
		-- Select rectangle.
		if self.selection_rect then
			for k,v in pairs(self.selection_rect) do
				self.selection[k] = v
				self.selection_rect[k] = nil
			end
			self:update_rect_select()
		end
	end
end

--- Enables or disables the rectangle selection mode.
-- @param self Editor.
-- @param value True to enable, false to disable.
Editor.set_rect_select = function(self, value)
	if not value == not self.selection_rect then return end
	if not value then
		for k,v in pairs(self.selection_rect) do
			v:detach()
			self.selection_rect[k] = nil
		end
		self.selection_rect = nil
	else
		self.selection_rect = {}
		self:update(0, true)
	end
end

--- Updates the editor state.
-- @param self Editor.
-- @param secs Seconds since the last update.
-- @param force True to force update.
Editor.update = function(self, secs, force)
	local h = self.highlight
	local s = self:pick()
	if h == s and not force then return end
	-- Update highlight.
	if h and not self.selection[h.key] then h:detach() end
	if s then self.highlight = s end
	-- Update the rectangle selection.
	self:update_rect_select()
end

Editor.update_corners = function(self)
	local i = 1
	local v = {}
	local face = function(a,b,c,d,n)
		v[i] = {a[1],a[2],a[3],n[1],n[2],n[3]}
		v[i+1] = {b[1],b[2],b[3],n[1],n[2],n[3]}
		v[i+2] = {d[1],d[2],d[3],n[1],n[2],n[3]}
		v[i+3] = v[i]
		v[i+4] = v[i+2]
		v[i+5] = {c[1],c[2],c[3],n[1],n[2],n[3]}
		i = i + 6
	end
	local o = -0.2
	local s = self.size * Voxel.tile_size - Vector(o,o,o)*2
	local p = {
		{o,o,o,1}, {s.x,o,o,1}, {o,s.y,o,1}, {s.x,s.y,o,1},
		{o,o,s.z,1}, {s.x,o,s.z,1}, {o,s.y,s.z,1}, {s.x,s.y,s.z,1}}
	-- Front and back.
	face(p[1], p[2], p[3], p[4], {0,0,-1})
	face(p[5], p[6], p[7], p[8], {0,0,1})
	-- Bottom and top.
	face(p[1], p[2], p[5], p[6], {0,-1,0})
	face(p[3], p[4], p[7], p[8], {0,1,0})
	-- Left and right.
	face(p[1], p[3], p[5], p[7], {-1,0,0})
	face(p[2], p[4], p[6], p[8], {1,0,0})
	-- Create the cube.
	self.corners.model:remove_vertices()
	self.corners.model:add_triangles{material = 1, vertices = v}
	self.corners.model:changed()
end

Editor.update_rect_select = function(self)
	if not self.selection_rect then return end
	-- Clear the selection.
	for k,v in pairs(self.selection_rect) do
		v:detach()
		self.selection_rect[k] = nil
	end
	-- Check for validity.
	local s = self.highlight
	local p = self.selection_prev
	if not s or not s.tile then return end
	if not p or not p.tile then return end
	if p.face ~= s.face then return end
	-- Create the new selection.
	local ord = function(a, b)
		return math.floor(math.min(a, b) + 0.5), math.floor(math.max(a, b) + 0.5)
	end
	local add = function(tile, face)
		-- Check for validity.
		local dir = Selection.face_dir[face]
		if Voxel:get_tile(tile) == 0 then return end
		if Voxel:get_tile(tile + dir) ~= 0 then return end
		-- Check if selected.
		local key = Selection:get_tile_key(tile, face)
		if self.selection[key] then return end
		-- Add a new selection.
		local s = Selection(tile, face)
		self.selection_rect[key] = s
	end
	if p.face == 1 or p.face == 2 then
		if p.tile.x == s.tile.x then
			local y1,y2 = ord(p.tile.y, s.tile.y)
			local z1,z2 = ord(p.tile.z, s.tile.z)
			for y=y1,y2 do
				for z=z1,z2 do
					add(Vector(p.tile.x, y, z), p.face)
				end
			end
		end
	elseif p.face == 3 or p.face == 4 then
		if p.tile.y == s.tile.y then
			local x1,x2 = ord(p.tile.x, s.tile.x)
			local z1,z2 = ord(p.tile.z, s.tile.z)
			for x=x1,x2 do
				for z=z1,z2 do
					add(Vector(x, p.tile.y, z), p.face)
				end
			end
		end
	else
		if p.tile.z == s.tile.z then
			local x1,x2 = ord(p.tile.x, s.tile.x)
			local y1,y2 = ord(p.tile.y, s.tile.y)
			for x=x1,x2 do
				for y=y1,y2 do
					add(Vector(x, y, p.tile.z), p.face)
				end
			end
		end
	end
end

------------------------------------------------------------------------------

-- Initialize the UI state.
Widgets.Cursor.inst = Widgets.Cursor(Iconspec:find{name = "cursor1"})
Editor.inst = Editor()
if not Editor.inst then
	Program.quit = true
end

-- Main loop.
while not Program.quit do
	-- Update program state.
	Program:update()
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
	-- Render the scene.
	Client:clear_buffer()
	Widgets:draw()
	Client:swap_buffers()
	-- Focus widgets.
	Widgets:update()
end
