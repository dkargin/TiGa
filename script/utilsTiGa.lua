-- utilsTiGa.lua
-- Core script
-- File contains different tools to simplify object creation

------------------------------------------------------
-- describes default hardpoint and writes custom fields
function _Mount(super)
	local result=
	{
		pose = Pose{0,0},		-- pose relative to unit
		bExtern = true,		-- placed externally on the hull
		auto = false,			-- true=device here is controlled by other device
		index = -1			-- mount index
	}
	for key,val in pairs(super) do
		result[key]=val
	end
	return result
end
------------------------------------------------------
-- make custom lua data for Device
DeviceProxy = function(manager)
	return
	{
		manager = manager,
		controls = {},
		onInit = function(self)
			if self.controls~=nil then
				for key,invoker in pairs(self.controls) do

				end
			end
		end,
		-- returns true if all we meet all requirements
		canMount = function(self, allow)
			for key,value in ipairs(allow) do
				if self.slots[value]==nil then return false end
			end
			return true
		end,
		onCreate=function(self,object) end,
		manualControl=function(self,unit,slot)	end,
	}
end

------------------------------------------------------
-- make custom lua data for UnitDef

UnitProxy=
{
	manager = function(self) return world.gameObjects end,
	_mounts = {},
	-- install device on object
	mount = function(self, key, device)
		local mount = self._mounts[key]
		if mount == nil then
			problem(1, "Wrong mount id:<"..key..">")
			return nil
		end
		local index = mount.index
		mount.device = device
		self.object:addDevice(device,index)
	end,
	initMounts = function(self)			
		local i = 0
		for mkey, mount in pairs(self._mounts) do
			if i == 0 then
				self.object:clearMounts()
			end
			mount.index = i
			if mount.device ~= nil then
				self.object:addMount(SWIG_construct(TiGa.MountDef,mount))							
			end
			i = i + 1
		end
	end,
	-- is called after UnitDef constructor, in Object2 proxy
	onInit=function(self) -- add all hardpoints to UnitDef. Also inserts devices, if there were any
		debugTrace("UnitProxy::onInit")
		self:initMounts()
	end,
	-- called when new Unit is created. <unit> contains hard pointer to unit.
	onCreate = function(self,unit)
		debugTrace("UnitProxy::onCreate")
	end,
}

function initHardObject(creatorName,proxy,default)
	-- check arguments
	if creatorName==nil then
		problem(Threat.wrongArgument,'initHardObject: wrong className')
		return nil
	end
	if proxy == nil then proxy = {} end
	if default == nil then default = {} end

	return function(super)
		trace('create object')
		local creator = TiGa[creatorName]
		if creator == nil then
			problem(Threat.wrongArgument,'initHardObject: wrong className = ' .. className)
			return nil
		end

		local class=creator(proxy.manager())
		if class==nil then
			problem(Threat.createFail,'initHardObject: Can not create')
			return {}
		end

		local softdata=SWIG_assign(class,mergeTables(super,default))

		if type(class.lua)=="number" then
			--debugTrace("storing additional data")
			softdata=mergeTables(softdata,proxy)
			print_table("custom",softdata,trace)
			class.lua=regStore(softdata)
			softdata.object=class
			softdata.source=super
		end

		if softdata.onInit~=nil then
			debugTrace("executing custom init")
			softdata:onInit(class)
		end

		return class
	end
end

function initSoftObject(creatorName,proxy,default)
	-- check arguments
	if creatorName==nil then
		problem(Threat.wrongArgument,'initSoftObject: wrong className')
		return nil
	end
	if proxy == nil then proxy = {} end
	if default == nil then default = {} end

	return function(super)

		local typedef = Types[creatorName]
		if typedef == nil then
			problem(Threat.wrongArgument,'initSoftObject: wrong className = ' .. className)
			return nil
		end

		local class = mergeTables(super,default)
		setmetatable(class, typedef)

		return class
	end
end

Slots = Enum{ "weapon", "engine", "external", "misc", "internal", "all" }

function InstallConstructors(initObject)
	function PerceptionManager(self)
		return world.gameObjects
	end
	function DeviceManager(self)
		return world.gameObjects
	end
	WeaponDef			= initObject("WeaponDef",DeviceProxy(DeviceManager), {slots = {Slots.weapon, Slots.external}})
	WeaponTurretDef		= initObject("WeaponTurretDef",DeviceProxy(DeviceManager), {slots = {Slots.weapon, Slots.external}})
	--Turret		= initObject(TiGa.TurretDef,DeviceProxy)
	MoverVehicleDef		= initObject("MoverVehicleDef",DeviceProxy(DeviceManager), {slots = List{Slots.engine, Slots.internal}})
	MoverCharacterDef	= initObject("MoverCharacterDef",DeviceProxy(DeviceManager), {slots = List{Slots.engine, Slots.internal}})
	ThrusterDef			= initObject("ThrusterDef",DeviceProxy(DeviceManager), {slots = List{Slots.engine, Slots.external}})
	ThrusterControlDef	= initObject("ThrusterControlDef",DeviceProxy(DeviceManager), {slots = List{Slots.engine, Slots.internal}})
	PerceptionDef		= initObject("PerceptionDef",DeviceProxy(PerceptionManager), {slots = List{Slots.misc, Slots.external}})
	ForceFieldDef		= initObject("ForceFieldDef",DeviceProxy(DeviceManager), {slots = List{Slots.misc, Slots.internal}})
	TargetingSystemDef	= initObject("TargetingSystemDef",DeviceProxy(DeviceManager), {slots = List{Slots.misc, Slots.internal}})

	ProjectileDef	= initObject("ProjectileDef",UnitProxy)
	UnitDef			= initObject("UnitDef",UnitProxy)
	ItemDef			= initObject("ItemDef",UnitProxy)
end

_RootEnv = _G
PacksRegistry = {}
-- loads pack
-- param0: path to pack
-- param1: creation policy (soft/hard)
function loadPack(packName, hard)
	local packPath = './script/' .. packName .. '.pack/'

	if _RootEnv.PacksRegistry[packName] then 
		print("previously loaded pack is found")
		return _RootEnv.PacksRegistry[packName]
	end
	-- link between group name and stored base type
	local Objects = 
	{
		Models = "FxEffect",
		Projectiles = "ProjectileDef",
		Items = "ItemDef",
		Devices = "DeviceDef",
		Units = "UnitsDef",
	}

	local packEnv = 
	{
		_RootEnv = getfenv(1)["_RootEnv"],
	}

	local pack =
	{
		name = packName,
		path = packPath,
		dependency = {},
		objects = {},
		-- pack environment during loading process
		env = setmetatable(packEnv,{__index = _G}),
		-- get object from specific group
		getObject = function(self, groupName, key)
			local fnName = self.name .. ":getObject"
			--print("getObject("..groupName..","..key..")")
			local group = rawget(self.objects,groupName)
			-- check if group is valid
			if group == nil then
				error(fnName .. ": invalid groupName = \'" .. groupName .. "\'")
				return
			end

			local result = rawget(group,key)
			if result then 
				return result 
			else
				for depName, depPack in pairs(self.dependency) do
					print(fnName .. " trying " .. depName)
					result = depPack:getObject(groupName,key)
					if result then
						return result
					end
				end
			end
			error(fnName .. ": unable to locate object " .. groupName .. "." .. key)
		end,

		getObjectRef = function(self, groupName, key)
			return { type = "ref", pack = self, group = groupName, key = key, value = rawget(self.objects[groupName],key) }
		end,

		enumCompatible = function(self,typename)
			if typename~=nil then
				--print("enumCompatible(" .. dumpdata(typename) ..")")
			else
				error('enumCompatible(nil)')
			end
			local typedef = Types[typename]
			print(dumpdata(typedef))
			local result = {}
			for source, rootType in pairs(Objects) do	
				if rootType == typename or typedef["base_full"][rootType] then
					for key, value in pairs(self.objects[source]) do
						table.insert(result, self:getObjectRef(source, key))
					end
				end
			end
			return result
		end,

		-- сохраняем содержимое пака
		save = function(self, path)
			-- сохраняем группу объектов
			function SaveObjects(objects, name)
				local result = name .. "\n{"
				for key, object in pairs(objects) do
					local typedef = SWIG_softtypedef(object)
					typedef:save(object,"")
				end
				return result ..  "}\n"
			end

			local output = io.open(path,"wb")
			for key, value in pairs(self.objects) do
				output:write(SaveObjects(value,key))
			end
			output:close()
		end,

		activate = function(self)
			for groupName, _ in pairs(Objects) do
				_G[groupName] = self.objects[groupName]
			end
			return self
		end,
	}

	for groupName, _ in pairs(Objects) do		
		local entry = pack.objects[groupName]
		if entry == nil then
			local meta = {}
			entry = {}
			meta.__call = function(_, source )
				print("merging group "..groupName)
				-- merge all contents from source table to this table
				for key, value in pairs( source ) do
					if rawget(entry,key) then
						print("overwriting existing object[" .. key .. "]")
					end
					rawset(entry,key,value)
				end
			end
			if hard then 
				meta.__index = function(_, key)
					print("Hard indexing " .. groupName .. "[" .. key .. "]")
					return pack:getObject(groupName,key)
				end
			else
				meta.__index = function(_, key)
					print("Soft indexing " .. groupName .. "[" .. key .. "]")
					return pack:getObjectRef(groupName,key)
				end
			end
			pack.objects[groupName] = entry
			setmetatable(entry, meta)
		end		
		packEnv[groupName] = entry
	end

	packEnv.include = function(path)
		local chunk,errorMsg = loadfile(packPath .. path)
		if chunk then
			print("including :".. packPath .. path)
			logger:levelInc()
			setfenv(chunk,packEnv)()
			logger:levelDec()
			print("including done")
		else
			error("include error:"..msg);
		end
	end

	packEnv.depends = function(depName)
		pack.dependency[depName] = loadPack(depName, hard)
	end

	local packMeta = 
	{
		__gc = function(self)
			print("pack " .. self.name .. " is unloaded")
		end,
	}
	setmetatable(pack,packMeta)

	if hard then
		InstallConstructors(initHardObject)
	else
		InstallConstructors(initSoftObject)
	end
	
	print("loadPack: executing pack script")

	local packChunk,errorMsg = loadfile(packPath .. 'pack.lua')
	if packChunk then		
		logger:levelInc()
		setfenv(packChunk,packEnv)()
		logger:levelDec()
		print("loadPack pack sript executed")
	else
		error("loadPack error:" .. errorMsg)
	end
	_RootEnv.PacksRegistry[packName] = pack
	return pack
end

-- creates item using definition <def> at <position,direction> for <player>
createItem = function(def,position,direction,player)
	local self=world
	debugTrace('-createItem')
	local item=self.gameObjects:create(Items[def]) --self.Items[def]:create()
	item:setPosition(position[1],position[2])
	item:setDirection(direction[1],direction[2])
	return item
end

-- creates unit, assigning control to it
createUnit = function(def,position,direction,owner,ai,aiArg)
	local self=world
	debugTrace('-createUnit')
	--local unit=self:addUnit(self.Units[def]:create())
	local object=self.gameObjects:create(PackHard.objects.Units[def])
	local unit=TiGa.toUnit(object)
	if unit~=nil then
		
		if owner==nil then 
			print("createUnit: no player is specified")
			owner=0 
		end
		unit:setPlayer(owner)
		if ai~=nil then
			unit:setController(ai(TiGa.toUnit(unit)))
		end
		unit:setPosition(position[1],position[2])
		unit:setDirection(direction[1],direction[2])
	end
	return unit
end

function CreateCamera(x,y,rotate,scale)
	if x == nil then x = 0 end
	if y == nil then y = 0 end
	if rotate == nil then rotate = 0 end
	if scale == nil then scale = 50 end
	return 
	{
		x = x,
		y = y,
		rotate = rotate,
		scale = scale,
		vx = 0,	-- camera velocity X
		vy = 0,	-- camera velocity Y
		va = 0,
		vs = 0,
		maxScale = 100,
		minScale = 0.1,
		followPos = false,
		followDirection = false,

		reset = function(self)
			self.x = 0.0
			self.y = 0.0
			self.scale = 50.0
			self.rotate = 0.0
			self.mode = self.modeFree
		end,

		
		modeFree = function(self, dt)
			self.x = self.x + self.vx * dt / self.scale
			self.y = self.y + self.vy * dt / self.scale
			self.scale = self.scale + self.vs * dt
			self.rotate = self.rotate * self.va * dt

			if self.scale > self.maxScale then
				self.scale = self.maxScale
			end

			if self.scale < self.minScale then
				self.scale = self.minScale
			end
		end,
		modeFollow = function(self, dt)
			local pose = self.target:getPose()
			if self.followPos then
				self.x = pose.x
				self.y = pose.y
			end
			if self.followDir then
				self.rotate = TiGa.RAD2DEG(pose.orientation)
			end
		end,

		modeGoto = function(self, dt)
			local delta = dt
			if self.targetTime < delta then
				delta = self.targetTime
				self.mode = self.modeFree
				print("camera arrived to")
			end
			self.targetTime = self.targetTime - delta
			self.x = self.x + self.vx * delta
			self.y = self.y + self.vy * delta
			self.scale = self.scale + self.vs * delta
			if self.targetTime < 0.01 then
				self.vx = 0
				self.vy = 0
				self.vs = 0
			end
		end,

		update = function(self, dt)
			if self.mode == nil then
				self.mode = self.modeGoto
			end
			self.mode(self,dt)
			world:setView(self.x,self.y,self.rotate,self.scale)
		end,

		follow = function(self, object, pos, dir)
			print("following ")
			self:detach()
			self.mode = self.modeFollow
			self.target = object
			self.followPos = pos
			self.followDir = dir
			DeadListeners[self] = object
		end,

		goto = function(self, x, y , scale, time)
			print("moving camera to")
			self:detach()
			self.mode = self.modeGoto
			self.targetTime = time
			self.vx = (x - self.x)/time
			self.vy = (y - self.y)/time
			self.vs = (scale - self.scale)/time
		end,

		isFollowing = function(self, object)
			if object == nil then
				return self.target ~= nil
			else
				return self.target == object
			end
		end,

		detach = function(self)
			if self.mode == self.modeFollow then
				self.target = nil
				DeadListeners[self] = nil
			end
			if self.mode == self.modeGoto then
				--self.targetPos = nil
				--self.targetScale = nil
				--self.targetTime = nil
				--self.currentTime = nil
			end
			self.mode = self.modeFree
		end,

		onUnitDie = function(self, object)
			if self.target:id() == object:id() then
				self:detach()
			end
		end,
	}
end