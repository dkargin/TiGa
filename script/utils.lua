-- utils.lua
-- Core script
-- File contains different tools to simplify object creation
function spairs(source)
	if type(source) == table then
		return pairs(source)
	else
		return function() end
	end
end
-- merge data from <super> and <default> and write to registry
-- simple way to print table contents.
function print_table(name,table,tracer)
	for q,v in pairs(table) do
		local t=type(v)
		local msg =''
		if t=='number' or t=='string' then
			msg=v
		elseif t=='function' then
			msg='<function>'
		elseif t=='table' then
			msg='<'..tostring(v)..'>'
		elseif t=='userdata' then
			msg='<userdata>'
		end
		tracer('>' .. name .. '[' .. q .. ']=' .. msg)
	end
end
-- dumps table data to string. Problems with 
function dumpdata(source,offset)
	local tab="  "
	local str=""
	if offset==nil then offset="" end
	if source==nil then
		return str.."nil\n"
	elseif type(source)=="function" then
		return str.."function\n"
	elseif type(source)=="userdata" then
		local meta=getmetatable(source)
		if meta~=nil then
			str=str.."metauserdata("..tostring(source)..")"..dumpdata(getmetatable(source),offset)
		else
			str=str.."userdata("..tostring(source)..")\n"
		end
	elseif type(source)=="table" then
		str=str.."\n"..offset.."{\n"
		for key,val in pairs(source) do
			str=str..offset..tab.."["..key.."]="..dumpdata(val,offset..tab)
		end
		local meta=getmetatable(source)
		if meta~=nil then
			str=str..offset..tab.."__meta="..dumpdata(getmetatable(source),offset..tab)
		end
		str=str..offset.."}\n"
	elseif type(source) == "string" then
		str=str.."\""..source.."\"\n"
	elseif type(source) == "boolean" then
		if source then
			str=str.."true".."\n"
		else
			str=str.."false".."\n"
		end
	else
		return str..source.."\n"
	end
	return str
end
-- dumps table data to string. Problems with 
function dumpsimple(source,offset)
	local tab="  "
	local str=""
	if offset==nil then offset="" end
	if source==nil then
		return str.."nil\n"
	elseif type(source)=="function" then
		return str.."function\n"
	elseif type(source)=="userdata" then		
		str=str.."userdata("..tostring(source)..")\n"
	elseif type(source)=="table" then
		str=str.."\n"..offset.."{\n"
		for key,val in pairs(source) do
			str=str..offset..tab.."["..key.."]="..dumpdata(val,offset..tab)
		end		
		str=str..offset.."}\n"
	elseif type(source) == "string" then
		str=str.."\""..source.."\"\n"
	elseif type(source) == "boolean" then
		if source then
			str=str.."true".."\n"
		else
			str=str.."false".."\n"
		end
	else
		return str..source.."\n"
	end
	return str
end

function dumpToFile(path, object)
	local str = dumpdata(object)
	local file = io.open(path,"wb")
	file:write(str)
	file:close()
end

local threatLevel=1
Threat=
{
	createFail=2,
	wrongArgument=1,
}
trace = debugTrace

function problem(level,message)
	if level>=threatLevel then
		error('Error:'..message..'\n'..debug.traceback())
	else
		trace('Warning:'..message)
	end
end

function Pose(x,y,rot,z)
	if type(x) == "table" and y == nil then return Pose(unpack(x)) end
	if z == nil then z = 0.0 end
	if rot == nil then rot = 0.0 end
	return TiGa.Pose(x, y, rot * TiGa.M_PI / 180, z)
end

function mergeTables(target,source)
	for key,val in pairs(source) do
		if target[key]==nil then target[key]=val end
	end
	return target
end

table.count = function (obj)
	local i = 0
	if obj~=nil then
		for key,value in pairs(obj) do
			i = i + 1
		end
	end
	return i
end


-- Compatibility: Lua-5.1
function split(str, pat)
   local t = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

function split_path(str)
   return split(str,'[\\/]+')
end

function DlgOpenFile(title, dir, extfilter)
	local result = ""
	local dlg = iup.filedlg
	{
		title = title,
		allownew = "no",
		dialogtype = "open",
		extfilter = extfilter,		
		fileexist = "yes",
		NOCHANGEDIR = "yes",
		DIRECTORY = dir,
	}
	dlg:popup()
	return dlg.value, dlg.status
end
--------------------------------------------------------
-- делает из списка имён {"obj1","obj2", ..., "objN" } таблицу
-- {obj1 = 1, obj2 = 2, ... ,objN = N}
-- таблица read-only. Выводит сообщение об ошибке при попытке достать некоректный элемент
function Enum(source)
	local result = {}
	for key,value in ipairs(source) do
		result[value] = key
	end
	local meta =
	{
		__index = function(self, key)
			local val = rawget(self,key)
			if val == nil then
				if key == nil then
					problem(Threat.wrongArgument,"Attempt to get wrong enum key: nil" )
				else
					problem(Threat.wrongArgument,"Attempt to get wrong enum key: " .. key )
				end
			end
			return val
		end,
		__newindex = function(self,key,value)
			problem(1,"Trying to write to enum data")
		end,
	}
	setmetatable(result,meta)
	return result
end


function List(source)
	local result = {}
	for key,value in ipairs(source) do
		result[value] = value
	end
	return result
end

exec('./script/utilsDynamics.lua')
exec('./script/utilsFx.lua')
exec('./script/RTTI.lua')

local tracer=nil

function setTracer(newTracer)
	tracer=newTracer
end

function AllItems(it, finish)	
	return function()
		local old_it = it
		if it ~= finish then
			it = TiGa.iteratorNext(it)
			return old_it
		else
			return nil
		end
	end
end