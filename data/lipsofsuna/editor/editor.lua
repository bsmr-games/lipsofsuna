require "editor/controls"
require "editor/editorcamera"
require "editor/editorobject"
require "editor/selection"

Editor = Class()

Editor.new = function(clss)
	-- Initialize self.
	local self = Class.new(clss)
	self.prev_tiles = {}
	self.mouse_sensitivity = 0.01
	self.selection = {}
	self.copybuffer = {}
	-- Camera and lighting.
	self.camera = EditorCamera()
	self.light = Light{ambient = {0.3,0.3,0.3,1.0}, diffuse={0.6,0.6,0.6,1.0}, equation={1.0,0.0,0.01}, priority = 2}
	self.scene = Widgets.Scene{camera = self.camera} -- FIXME
	return self
end

Editor.reset = function(self)
	self.prev_tiles = {}
	self.selection = {}
	self.copybuffer = {}
	if self.corners then
		self.corners.realized = false
		self.corners = nil
	end
end

--- Initializes the editor mode.
-- @param self Editor.
Editor.initialize = function(self)
	self.initialized = true
	if Settings.pattern then
		self:load(Settings.pattern)
		Settings.pattern = nil
	end
	self.light.enabled = true
	Client.sectors.unload_time = nil
	if Map then Map:init() end
end

--- Uninitializes the editor mode.
-- @param self Editor.
Editor.uninitialize = function(self)
	self.initialized = nil
	for k,v in pairs(Object.objects) do v:detach() end
	Client.sectors:unload_world()
	self.light.enabled = false
	Client.sectors.unload_time = 10
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

Editor.grab = function(self, delta)
	-- Get the view direction of the ray.
	local look = self.camera.rotation * Vector(0,0,-1)
	-- Transform each selected object.
	for k,v in pairs(self.selection) do
		if v.object then
			-- Project camera axes to the selection plane.
			local normal = v.face_dir[v.face]
			local camerax = self.camera.rotation * Vector(1, 0, 0)
			local cameray = self.camera.rotation * Vector(0, 1, 0)
			local movex = camerax - normal * camerax:dot(normal)
			local movey = cameray - normal * cameray:dot(normal)
			-- Multiply the projected axes by mouse movement factors.
			movex = movex * (0.5 * -delta.x)
			movey = movey * (0.5 * -delta.y)
			-- Scale the axes by the distance between the object and the camera.
			local scale = (self.camera.position - v.object.position).length
			movex = movex * scale
			movey = movey * scale
			-- Move the selection.
			v:transform(Vector(), movex + movey, Quaternion())
			v:refresh()
		end
	end
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

Editor.load = function(self, name)
	-- Make sure the old map is erased.
	self:reset()
	for k,v in pairs(Object.objects) do v:detach() end
	Client.sectors:unload_world()
	-- Find or create the pattern.
	local pattern = Patternspec:find{name = name}
	if not pattern then
		pattern = Patternspec{name = name, file = name .. ".lua", size = Vector(32,8,32)}
		local i = 1
		for x=0,31 do
			for z=0,31 do
				pattern.tiles[i] = {x,0,z,"granite1"}
				i = i + 1
			end
		end
	end
	-- Load the pattern.
	self.pattern = pattern
	self.origin = pattern.position or Vector(100,100,100)
	self.size = pattern.size or Vector(10,10,10)
	Voxel:place_pattern{class = EditorObject, point = self.origin, name = name}
	-- Setup the camera.
	self.camera:warp((self.origin + self.size * 0.5) * Voxel.tile_size, Quaternion(0, 1, 0, 0))
	-- Corner markers.
	if not self.corners then
		self.corners = EditorObject{position = self.origin * Voxel.tile_size, realized = true}
	end
	self.corners.model = Model()
	self.corners.model:add_material{material = "bounds1"}
	self:update_corners()
	-- Update the map name entry.
	self.map_name = name
end

Editor.copy = function(self, args)
	self.copybuffer = deepcopy(self.selection)
end

Editor.paste = function(self, args)
	--FIXME make objects spawn at cursor
	--FIXME Tile copying
	--TODO make pasted objects current selection
	local point,object,tile = Target:pick_ray{camera = self.camera}
	if self.highlight and self.selection[self.highlight.key] then point = self.selection[self.highlight.key].tile end
	local prevpoint = nil
	
	--Calculate offset of pastebuffer objects
	for k,v in pairs(self.copybuffer) do
		
		if v.object and v.object.position then
			if not prevpoint then prevpoint = v.object.position v.offset = Vector(0,0,0)
			else
				v.offset = prevpoint - v.object.position
			end
		end
		if v.tile then
			if not prevpoint then prevpoint = v.tile v.offset = Vector(0,0,0)
			else
				v.offset = prevpoint - v.tile
			end
		end
	end
	
	--insert pastebuffer objects into world
	for k,v in pairs(self.copybuffer) do
		if v.object and point then
			--if self.selection[1].object and then print "bas" point=self.selection[1].object.position end
			EditorObject{position = point+v.offset, realized = true, spec = v.object.spec}
		elseif v.tile and point then
		--local mat = v.tile.material
		local t,p = Voxel:find_tile{point = point+v.offset}
		if p then
			Voxel:set_tile(p, Voxel:get_tile(v.tile))
		end 
		end
		
	end
end

--- Creates an actor.
-- @param self Editor.
-- @param name Object spec name.
Editor.create_actor = function(self, name)
	-- Find the insertion point.
	local point,object,tile = self:pick_scene()
	if not point then return end
	-- Find the inserted object type.
	local spec = Actorspec:find{name = name}
	if not spec then return end
	-- Create the object.
	EditorObject{position = point, realized = true, spec = spec}
end

--- Creates an item.
-- @param self Editor.
-- @param name Object spec name.
Editor.create_item = function(self, name)
	-- Find the insertion point.
	local point,object,tile = self:pick_scene()
	if not point then return end
	-- Find the inserted object type.
	local spec = Itemspec:find{name = name}
	if not spec then return end
	-- Create the object.
	EditorObject{position = point, realized = true, spec = spec}
end

--- Creates an obstacle.
-- @param self Editor.
-- @param name Object spec name.
Editor.create_obstacle = function(self, name)
	-- Find the insertion point.
	local point,object,tile = self:pick_scene()
	if not point then return end
	-- Find the inserted object type.
	local spec = Obstaclespec:find{name = name}
	if not spec then return end
	-- Create the object.
	EditorObject{position = point, realized = true, spec = spec}
end

--- Creates a static object.
-- @param self Editor.
-- @param name Object spec name.
Editor.create_static = function(self, name)
	-- Find the insertion point.
	local point,object,tile = self:pick_scene()
	if not point then return end
	-- Find the inserted object type.
	local spec = Staticspec:find{name = name}
	if not spec then return end
	-- Create the object.
	EditorObject{position = point, realized = true, spec = spec}
end

--- Creates a tile.
-- @param self Editor.
-- @param name Tile spec name.
Editor.create_tile = function(self, name)
	-- Find the insertion point.
	local point,object,tile = self:pick_scene()
	if not point then return end
	-- Find the inserted tile type.
	local mat = Material:find{name = name}
	if not mat then return end
	-- Find an empty tile.
	local t,p = Voxel:find_tile{match = "empty", point = point}
	if not p then return end
	-- Fill the tile.
	Voxel:set_tile(p, mat.id)
	self.prev_tiles[2] = self.prev_tiles[1]
	self.prev_tiles[1] = p
end

--- Deletes an item or obstacle.
-- @param self Editor.
Editor.delete = function(self)
	-- Find the active object or tile.
	local point,object,tile = self:pick_scene()
	-- Delete the object or tile.
	if object then
		object:detach()
	elseif tile then
		Voxel:set_tile(tile, 0)
	end
end

Editor.pressed = function(self, args)
end

Editor.save = function(self)
	local items = {}
	local obstacles = {}
	local actors = {}
	local statics = {}
	-- Collect objects.
	for k,v in pairs(Object.objects) do
		if v.realized and v.spec then
			if v.spec.type == "item" then
				table.insert(items, v)
			elseif v.spec.type == "obstacle" then
				table.insert(obstacles, v)
			elseif v.spec.type == "actor" then
				table.insert(actors, v)
			elseif v.spec.type == "static" then
				table.insert(statics, v)
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
	table.sort(actors, sortobj)
	table.sort(statics, sortobj)
	-- Update the pattern.
	local makeobj = function(v)
		local p = roundvec(v.position)
		local r = v.rotation.euler
		local e
		if r[1] ~= 0 then
			e = math.floor(100 * r[1] / (2 * math.pi) + 0.5) / 100
			if e == -0.5 then e = 0.5 end
		end
		return {p[1], p[2], p[3], v.spec.name, e}
	end
	self.pattern.items = {}
	for k,v in pairs(items) do
		self.pattern.items[k] = makeobj(v)
	end
	self.pattern.obstacles = {}
	for k,v in pairs(obstacles) do
		self.pattern.obstacles[k] = makeobj(v)
	end
	self.pattern.actors = {}
	for k,v in pairs(actors) do
		self.pattern.actors[k] = makeobj(v)
	end
	self.pattern.statics = {}
	for k,v in pairs(statics) do
		self.pattern.statics[k] = makeobj(v)
	end
	local i = 1
	self.pattern.tiles = {}
	for z = 0,self.size.z-1 do
		for y = 0,self.size.y-1 do
			for x = 0,self.size.x-1 do
				local v = Voxel:get_tile(self.origin + Vector(x, y, z))
				if v ~= 0 then
					local mat = Material:find{id = v}
					if mat then
						self.pattern.tiles[i] = {x, y, z, mat.name}
						i = i + 1
					end
				end
			end
		end
	end
	-- Write to the file.
	if not self.pattern:write_file() then
		if not self.pattern:write_file_new(self.pattern.file) then
			print(string.format("ERROR: Could not save `%s'.", name))
		end
	end
end

Editor.pick_scene = function(self)
	local mode = Program.video_mode
	local cursor = Vector(mode[1]/2, mode[2]/2)
	local r1,r2 = self.camera:picking_ray{cursor = cursor, far = 20, near = 0.1}
	local ret = Physics:cast_ray{src = r1, dst = r2}
	if not ret then return end
	return ret.point, ret.object, ret.tile
end

Editor.pick = function(self)
	-- Pick from the scene.
	local hl = self.highlight and self.highlight.visual
	local point,object,tile = self:pick_scene()
	if not point then return end
	-- Find or create a selection.
	if object then
		--determine the object face
		local osize = object.bounding_box_physics.size
		local c = self.camera.position
		local objs = c - object.position --Vector(osize.x/2,osize.y/2,osize.z/2))
		local objsx = math.abs(objs.x)
		local objsy = math.abs(objs.y)
		local objsz = math.abs(objs.z)
		--print(self.camera.position,objsz,objsy,objsx)
		local face
		if objsx > objsy and objsx > objsz then
			face = objs.x < 0 and 1 or 2
		elseif objsy > objsz then
			face = objs.y < 0 and 3 or 4
		else
			face = objs.z < 0 and 5 or 6
		end
		--print(face)

-- 		local c = self.camera.position
-- 		local op = object.position
-- 		local face
-- 		if c.x - op.x > c.x - op.x
-- 		if c.x > op.x then face = 2 else face = 1 end 
		
		if object.selection then return object.selection end
		local i = object
		local se = self.selection[i]
		if se then return se end
		return Selection(object,face)
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
	-- Update the camera.
	local mode = Program.video_mode
	local cp0 = self.camera.target_position
	local cr0 = self.camera.rotation
	self.camera:update(secs)
	self.scene:update_camera()
	local cp1 = self.camera.target_position
	local cr1 = self.camera.rotation
	-- Update the light source.
	self.light.position = cp1 + cr1 * Vector(0,0,-5)
	-- Pick the scene.
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
--simplified selection to select in a cube instead of a plane
	local x1,x2 = ord(p.tile.x, s.tile.x)
	local y1,y2 = ord(p.tile.y, s.tile.y)
	local z1,z2 = ord(p.tile.z, s.tile.z)
	for x=x1,x2 do
		for y=y1,y2 do
			for z=z1,z2 do
				add(Vector(x, y, z), p.face)
			end
		end
	end
end

function makeGridSelect(parent,items)
		-- Crafting actions.
		local pressed = function(w)
			parent.itemselected=w.text
		end
		local scrolled = function(w, args)
			crafting:scrolled(args)
		end
		-- Build the crafting item buttons.
		craftable = {}
		for k,v in ipairs(items) do
			local spec = Itemspec:find{name = v}
			--if Crafting:can_craft({spec=spec}) then print("can") end
			local widget = Widgets.ItemButton{enabled = true, id = id,
				index = k, icon = spec and spec.icon, spec = spec, text = v,
				pressed = pressed, scrolled = scrolled}
			table.insert(craftable, widget)
		end
		-- Pack the crafting list.
		crafting = Widgets.List()
		local col = 1
		local row = Widget{cols = 8, rows = 1, spacings = {0,0}}
		for k,v in ipairs(craftable) do
			row:set_child(col, 1, v)
			if col == row.cols then
				crafting:append{widget = row}
				row = Widget{cols = row.cols, rows = 1, spacings = {0,0}}
				col = 1
			else
				col = col + 1
			end
		end
		if row.cols > 0 then
			crafting:append{widget = row}
		end
		-- Add the list to the container widget.
		return crafting
end

function deepcopy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end
