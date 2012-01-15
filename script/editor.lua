function InsertWnd(parent,wnd)
	local res = iup.Append(parent,wnd)
	iup.Map(wnd)
	iup.Refresh(res)
end

-------------------------------------------------
-- conversion for iup.toggle
-- {false,true} -> {"no","yes"}
function bool2iup(value)
	if value then
		return "yes"
	else
		return "no"
	end
end
-------------------------------------------------
-- conversion for iup.toggle
--  {"0","1"} -> {false,true}
function iup2bool( value )
	if value == 1  then
		return true
	else
		return false
	end
end

-- тут лежит сам редактор
Editor = {}

-- таблица с открытыми объектами, дабы не редактировать дважды
Editor.Revealed = {}

function AppendWindow(container, control)
	iup.Append(container,control)
	iup.Map(control)
	iup.Map(container)
	iup.Refresh(container)
end

function RemoveChildren(container)
	local count = iup.GetChildCount(container)
	print("Removing " .. count .. " child windows")
	for i = 1, count do
		iup.Detach(container[1])
		print("element " .. i .. " removed")
	end
end

Revealers = 
{
	-- create control for single value carrier
	value = function(object, member)
		print_table("Revealers.value", member, print)
		local data = object[member.name]
		-- игнорируем поле lua (не дай бог кто его пофиксит) и name (редактируем в другом месте)
		local fieldtype = member.type
		if fieldtype == nil then
			print("Reveal: invalid type <".. member.type.name .. "> for member <" .. member.name .. ">")
		end
		-- create control
		local fieldEditor = fieldtype:createControl(object, member.name)
		-- fill control
		fieldEditor:updateDown()
		-- create control for single value
		local result = BasicControl(object, member.name)
		{ 
			control = iup.hbox
			{
				iup.label{title = member.name, size = "100x"},
				--iup.label{title = member.type, size = "100x"},	-- temporary field
				fieldEditor.control
			},
			fieldEditor = fieldEditor,
			updateDown = function(self)
				self.fieldEditor:updateDown()
			end,
		}
		fieldEditor.parent = result
		return result
	end,
	-- create control for list carrier
	list = function(object, member)
		print_table("Revealers.list member", member, print)
		local data = object[member.name]
		-- игнорируем поле lua (не дай бог кто его пофиксит) и name (редактируем в другом месте)					
		--print("coretype is " .. coretype)
		local fieldtype = member.type
		
		local result = BasicControl(object, member.name)
		{	
			created = false,				
			clear = function(self)
				-- remove all objects
				for key, value in ipairs(self) do			
					--iup.Detach(value.itemContainer)
					self[key] = nil
				end
			end,
			remove = function(self, key)	
				local value = self:value()
				value[key] = nil
				self:updateDown()
			end,
			updateDown = function(self)
				-- rebuild list from scratch
				local list = self.control[1]
				if self.created then
					print("clearing list contents")
					iup.Detach(list)
					iup.Map(self.control)
					iup.Refresh(self.control)					
					self:clear()
					list = iup.vbox{}
					AppendWindow(self.control, list)
					print("cleared")
				end
				
				self.created = true
				local value = self:getValue()
				if value then
					for key, value in pairs(data) do
						local fields = {}
						local control = fieldtype:createControl(data, key)
						local itemContainer = iup.hbox 
						{
							iup.button
							{
								title = "del",
								action = function(_)
									self:remove(key)
								end,
							},
							control.control,					
						}
						control.parent = self
						print("inserting new item")			
						AppendWindow(list, itemContainer)
						self[key] = control
						print("inserted")
					end
				end
				
			end,
		}		
		result.control = iup.frame
		{
			title = member.name .. " of " .. fieldtype.name,
			iup.vbox{},			
		}
		return result
	end,
	-- reference to some object
	reference = function(object, member)
	end,
}

-------------------------------------------------
-- создаём контролл - таблицу с содержимым объекта
-- name | value
-- item0| controller
-- ...
Editor.RevealObject = function(self,object,name)
	print("Editor.RevealObject")
	local typedef = SWIG_softtypedef(object)
	if typedef == nil then
		problem(Threat.wrongArgument,'RevealObject: no SWIG typedef table')
		return
	end
	local controls = {}
	local result = {items = {}}
	-- собираем список элементов управления
	for _,member in ipairs(typedef) do
		local key = member.name
		if key ~= "lua" and key ~= "name" and member.get and member.set then
			local revealer = Revealers[member.carrier]
			print("Editor.RevealObject: revealing " .. key .. " of " .. member.type.name)
			if revealer then
				local wnd = revealer(object, member)
				table.insert(controls, wnd.control)
				table.insert(result.items, wnd)
				wnd:updateDown()
				wnd.parent = result
			else
				print("RevealObject: no valid revealer found for " .. name .. "[" .. key .. "]")
			end
		end
	end
	-- создаём непосредственно редактор
	result.control = iup.frame
	{
		iup.hbox
		{
			iup.vbox(controls),
			expand="HORIZONTAL",
			gap = 2,
		},title = name .. ":" .. GetType(object)
	}
	result.updateUp = function(self)
		if self.parent then
			self.parent:updateUp()
		end
	end
	result.updateDown = function(self)
		for key, value in pairs(self.items) do
			value:updateDown()
		end
	end
	print("Editor.RevealObject complete")
	return result
end

function MakeObjectWindow(Pack)
	local EnumObjects = function(objects,name)
		local source = {branchname = name}
		for key, value in pairs(objects) do
			local item =
			{
				leafname = key,
				userid = {key,value},
			}
			table.insert(source,item)
		end
		return source
	end

	tree = iup.tree
	{
		ADDEXPANDED= "yes",ADDROOT = "yes",
		size = "THIRDxFULL",
	}
	-- Creates menu displayed when the right mouse button is pressed
	addleaf = iup.item {title = "Add Leaf"}
	addbranch = iup.item {title = "Add Branch"}
	renamenode = iup.item {title = "Rename Node"}
	menu = iup.menu{addleaf, addbranch, renamenode}

	-- Callback of the right mouse button click
	function tree:rightclick_cb(id)
	  tree.value = id
	  menu:popup(iup.MOUSEPOS,iup.MOUSEPOS)
	end

	function tree:selection_cb(id, state)
		if state == 1 then
			local container = Editor.dataHolder

			if Editor.ActiveRevealed then
				iup.Detach(Editor.ActiveRevealed)
				Editor.ActiveRevealed = nil
			end

			local data = iup.TreeGetUserId(tree,id)

			if data then
				local name, object = unpack(data)
				local control = Editor:RevealObject(object, name)
				iup.Append(container,control)
				iup.Map(control)
				iup.Map(container)
				iup.Refresh(container)
				Editor.ActiveRevealed = control
			end
			--self.Revealed[object] = result
		end
	end

	Editor.dataHolder = iup.hbox
	{
		margin = "5x5",
		iup.frame
		{
			title = "Objects",
			iup.vbox
			{
				size = "THIRDxFULL",
				tree,
			},
		},
		Editor.dataHolder,
	}

	local menu = iup.menu
	{
	}
	local result = iup.dialog
	{
		size = "FULLxFULL",
		title = "Objects library",
		Editor.dataHolder,
		menu = iup.menu
		{
			iup.submenu
			{
				iup.menu
				{
					iup.item
					{
						title = "Open", key = "K_O",
						action = function(self)
							return iup.DEFAULT
						end,
					},
					iup.item
					{
						title = "Save", key = "K_S",
						action = function(self)
							SavePack(Pack, "savetest.pack")
							return iup.DEFAULT
						end,
					},
					iup.item
					{
						title = "Exit", key = "K_E",
						action = function(self)
							return iup.CLOSE
						end,
					},
				},
				title = "File",
			},

		},
	}

	tree:map()
	result:map()

	tree.name = "Objects"
	local roots = {}

	for groupname, group in pairs(Pack.objects) do
		table.insert(roots,EnumObjects(group,groupname))
	end

	iup.TreeAddNodes(tree, {unpack(roots)})
	tree.value = "6"
	return result, dataHolder
end