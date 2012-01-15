-- RTTI.lua
-- Core script
-- runtime type info

-- тип носителя
CarrierType = Enum { value, reference }
-- таблица типов. Здесь храним действия для чтения и записи типов
Types = {}

setmetatable(Types,
{
	__index = function(self, name)
		local value = rawget(self,name)
		if value == nil then
			print("No type " .. name .. " is found")
		end
		return value
	end,
})

-- создаём новый тип
function MakeType(typename)
	if type(typename) ~= "string" then
		error("MakeType: Invalid type name")
	end
	return 
		function(source)
			if source.check == nil then
				source.check = function(self, owner, ownerKey)
					if owner[ownerKey] == nil then
						self:default(owner, ownerKey)
					end
				end
			end			
			source.name = typename
			Types[typename] = source
		end
end

function ClassifyValue(typename)
	local coretype = SWIG_base_name(typename)
	if coretype == typename then
		return coretype, "value"
	else
		return coretype, "reference"
	end
end

function SaveMemberReference(reference, typedef, offset )	
	if reference and reference.key and reference.group then
		return reference.group .. "." .. reference.key
	else
		error("SaveMemberReference: reference is invalid") 
	end
end

-- сохраняем поле-значение
function SaveMemberValue(value, typedef, offset)
	if value == nil then error("SaveMemberValue: value is nil") end
	if typedef == nil then error("SaveMemberValue: typedef is nil") end
	if typedef then
		return typedef:save(value, offset)
	else
		print("invalid field_typedef")
	end	
end

function SaveMemberList(list, typedef, offset)
	print("SaveMemberList")
	if list == nil then error("SaveMemberList: list is nil") end
	if typedef == nil then error("SaveMemberList: typedef is nil") end

	local empty = true
	local result = "\n" .. offset .. "{\n"

	for _, value in ipairs(list) do
		print("SaveMemberList iteration")
		empty = false									
		result = result .. offset .. "\t" .. typedef:save(value, offset .. "\t") .. ",\n"
	end

	if empty then
		return "{}"
	else
		return result .. offset .. "}"
	end
end

function MakeComplexType(typename)
	if type(typename) ~= "string" then
		error("MakeType: Invalid type name")
	end
	return 
		function(source)
			-- поидее у всех твёрдых тайпдефов должна уже быть ссылка на мягкий вариант. 
			-- Если его нет - значит эта таблица является луа типом (мягким)
			if source.soft == nil then
				source.softTypedef = true
			end
			-- функция резетит объект к дефолтному состоянию
			if source.check == nil then
				source.check = function(self, owner, ownerKey)
					if owner[ownerKey] == nil then
						self:default(owner, ownerKey)
					end
				end
			end
			-- собираем таблицу [field name] -> member desc
			if not source.members then
				print("MakeType: building member table")
				source.members = {}
				for key, value in ipairs(source) do
					if type(value) == "table" then
						source.members[value.name] = value
					else
						print("MakeType: invalid member entry")
					end
				end
			end
			-- сохранение объекта в строчку. Проходим по всем полям и формируем строку в формате луа таблицы
			source.save = function(self, value, offset)
				if offset == nil then offset = "" end
				local result = ""				
				local typedef = self --SWIG_softtypedef(object)	
				result = typedef.name .. "\n" .. offset .. "{\n"
				-- собираем список элементов управления
				for _,member in ipairs(typedef) do
					local key = member.name
					-- игнорируем поле lua (не дай бог кто его пофиксит) и name (редактируем в другом месте)
					if key ~= "lua" and key ~= "name" and member.set and member.get and value[key] then
						if key == nil then
							print("something strange:" .. dumpdata(member).. " of " .. _)
						else
							if member.carrier == nil then member.carrier = "value" end
							print("saving " .. member.carrier .. " " .. key .. " of " .. member.type.name)
						end
						savers = 
						{
							reference = SaveMemberReference,
							value = SaveMemberValue,
							list = SaveMemberList,
						}						
						result = result .. offset .. "\t" .. key .. " = " .. savers[member.carrier](value[key], member.type, offset .. "\t") .. ",\n"
						
					end
				end
				-- записываем кастомные данные
				if type(source.save_custom)=="function" then
					print("saving custom block")
					result = result .. source:save_custom(value, offset .. "\t") .. ",\n"				
				end
				result = result .. offset .. "}"
				return result
			end
			source.name = typename
			Types[typename] = source
		end
end
--------------------------------------------------------------
-- делаем ссылку на уже существующий тип
function MakeTypeAlias(typename,alias)
	Types[alias] = Types[typename]
end

----------------------------------------------------
-- проверяем, существует ли тип
function TypeExists(typename)
	return Types[typename] ~= nil
end

----------------------------------------------------
-- Возвращает описание SWIG-типа
function SWIG_typedef(object)
	if object == nil then error("SWIG_typedef: nil argument") end
	return getmetatable(object)
end

----------------------------------------------------
-- Получаем описание луа аналога SWIG типа
function SWIG_softtypedef(object)
	if object == nil then error("SWIG_softtypedef: nil argument") end
	local meta = getmetatable(object)
	if meta.softTypedef == true then
		return meta
	else
		return meta.soft
	end
end
----------------------------------------------------
-- Проверяем наличие SWIG-типа
-- Проходим по разделу .set,
function SWIG_assign(object,source)
	local d_trace= function()end
	d_trace("SWIG_assign{")
	local entry = "get"
	local typedef = SWIG_typedef(object)
	if typedef==nil then
		problem(Threat.wrongArgument,'SWIG_assign: no SWIG typedef table')
		return
	end
	--d_trace("type=" .. dumpdata(typedef))
	local softobject={}
	local getters = typedef.get
	if getters~=nil then
		-- идём от source
		for name,value in pairs(source) do
			if getters[name]~=nil then	-- есть в таблице имён
				d_trace("object["..name.."]")
				object[name]=value
			else
				d_trace("softobject["..name.."]")
				softobject[name]=value
			end
		end
	end
	d_trace("}SWIG_assign")
	return softobject
end

----------------------------------------------------
-- Проверяем наличие SWIG-типа
-- Проходим по разделу .set,
function SWIG_construct(constructor,source)
	local d_trace= function()end
	d_trace("SWIG_construct{")
	local entry = "get"
	local object = constructor()
	local typedef = SWIG_typedef(object)
	if typedef==nil then
		problem(Threat.wrongArgument,'SWIG_construct: no SWIG typedef table')
		return
	end
	local getters = typedef.get
	if getters~=nil then
		-- идём от source
		for name,value in pairs(source) do
			if getters[name] then	-- есть в таблице имён				
				object[name]=value
			end
		end
	end
	d_trace("}SWIG_construct")
	return object
end

function SWIG_type(object)
	local t = type(object)
	if t == "userdata" then
		local typedef = SWIG_typedef(object)
		if typedef ~=nil then
			return typedef.name
		end
	end
	return "unknown"
end

function GetType(object)
	local t = type(object)
	if t == "userdata" then
		local typedef = SWIG_typedef(object)
		if typedef ~=nil then
			return typedef.name		
		end		
	end
	return t
end

-- tests if object is compatible with typename (derived or exact)
-- returns bool
function isKindOf(object, typename)
	local typedef = SWIG_softtypedef(object)
	-- exact type match
	if typedef.name == typename then
		return true
	end
	-- test parent
	return (typedef.base_full[typename] ~= nil)
end

function BasicControl(owner,ownerKey)
	return function(source)
		source.owner = owner
		source.ownerKey = ownerKey
		-- send update up to the top of hierarchy
		if source.updateUp == nil then
			source.updateUp = function(self, newValue) 
				if self.parent then
					self.parent:updateUp()
				else
					print("BasicControl:updateUp - no valid parent was provided")
				end
			end
		end

		if source.updateDown == nil then
			source.updateDown = function(self, newValue) end
		end
		
		source.getValue = function(self)
			return self.owner[self.ownerKey]
		end
		return source
	end
end

function CompileSoftTypedef(typedef, result)
	--result[".membertype"] = typedef[".membertype"]
	function NameToTypedef(source)
		local res = {}
		for key, value in pairs(source) do
			res[key] = Types[value]
		end
		return res
	end
	result.base = NameToTypedef(typedef.base)
	result.children = NameToTypedef(typedef.children)
	result.base_full = NameToTypedef(typedef.base_full)
	result.children_full = NameToTypedef(typedef.children_full)

	result.members = {}
		
	for key, value in pairs(typedef.members) do
		local member = 
		{
			name = value.name,
			type = value.type,
		}
		member.set = (value.set~=nil)
		member.get = (value.get~=nil)
		-- assume that key is int
		result[key] = member
		result.members[value.name] = member
	end
end

-- read SWIG type table and generate lua emulation
function GenerateSoftTypedef(typedef, result)
	result.default = function(self, owner, ownerKey)
	end
	result.save = function(self, value)
		return "{some object}"
	end
	result.type = typedef.type	
	result.members = {}
	result.softTypedef = true

	result.createControl = function (self, owner, ownerKey)			
		print("createControl(...,"..ownerKey..") of ".. self.type)
		-- maps listbox index to target object
		local idMap = {}
		print_table("owner["..ownerKey.."]",owner[ownerKey],print)
		--print(dumpdata(owner[ownerKey]))
		-- fill in listbox
		UpdateList = function (control, selection)
			--print("UpdateList "..dumpdata(value))
			local coretype = SWIG_base_name(self.type)
			--print("type = " .. dumpdata(coretype))
			
			local group = PackSoft:enumCompatible(coretype)
			-- add default values
			control[1] = "NONE"
			idMap[1] = "NONE"
			-- select NONE by default
			control.value = 1 			
			-- fill in available objects list
			if group ~= nil then
				--print("value = "..dumpdata(group))
				local i = 2
				for key, val in pairs(group) do
					print_table("variant["..i.."]",val,print)
					control[i] = val.group .. "." .. val.key
					idMap[i] = val
					-- we got reference here, so compare key name and group name
					if selection and val.group == selection.group and val.key == selection.key then
						print("equal found")
						control.value = i
					end
					i = i + 1
				end				
				iup.Refresh(control)
			end
		end

		local container = iup.list
		{
			dropdown = "yes",
			expand="HORIZONTAL",
			-- generated when we try to choose an object			
			valuechanged_cb = function(self)
				local index = tonumber(self.value)
				if index and index > 1 then
					owner[ownerKey] = idMap[index].value
				else
					self:default(owner, ownerKey)
				end
				return iup.DEFAULT
			end
		}
		UpdateList(container, owner[ownerKey])
		local result = BasicControl(owner,ownerKey)
		{
			control = iup.hbox
			{
				iup.text
				{
					expand="HORIZONTAL",
					readonly = "yes",
					value = GetType(value),
				},
				container,
				iup.button
				{
					title = "goto",
					action = function(self)
						return iup.DEFAULT
					end,
				}
			}
		}
		print("createControl complete")
		return result
	end
	return result
end

-- analyse SWIG typetable
function BuildClassTree(types)
	-- result = {<name> = <{typedef}>, ... }
	for key, value in pairs(types) do
		-- build children lists
		for baseName,_ in pairs(value.base) do
			types[baseName].children[key] = key
		end
		-- build full children lists
		for baseName,_ in pairs(value.base_full) do
			types[baseName].children_full[key] = key
		end
		value.soft = {}
		GenerateSoftTypedef(value,value.soft)
		if TypeExists(key) then
			print("the type <" .. key .. "> is already registered")
		else
			MakeComplexType(key)(value.soft)
		end
	end
	for key, value in pairs(types) do
		CompileSoftTypedef(value, value.soft)
	end
end


MakeMember = function(name, typedef, carrier)
	if not carrier then carrier = "value" end
	return { name = name, type = typedef, get = true, set = true, carrier = carrier }
end



function SoftObject(typename)
	return function(source)
		local typedef = Types[typename]
		if typedef then			
			setmetatable(source,typedef)
			return source
		else
			trace("SoftObject: cannot find type \'" .. typename .. "\'") 
		end		
	end
end

exec("./script/types.lua")



BuildClassTree(swig_typemap())
--dumpToFile("softtypes.txt",Types)