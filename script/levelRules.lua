--
--This file is packed in every level. Here are described game rules, like FFA, TFFA, CFT
randRange=TiGa.randRange
PlayerID=
{
	neutral=0,
	berserk=1,
	player=2,
	NPC=3,	
}

-------------------------------------------------------
-- @param0 slotType - slot classification
-- @param1 current - current device
function enumDevices(slotType, current)
	local result =
	{
		dropdown = "yes", expand="HORIZONTAL", editbox = "no", multiple = "no",
	}
	local i = 0

	for _,device in pairs(Devices) do
		local customData = regGet(device.lua)
		if customData:canMount(slotType) then
			table.insert(result,device.name)
			i = i + 1
			if device.name == current.name then
				result.value = i
			end
		end
	end
	return iup.list(result)
end

function CustomizeUnit(definition)
	local customData = regGet(definition.lua)
	local list = {}
	-- enum all mounts

	for key,value in pairs(customData._mounts) do
		-- skip locked mounts
		if value.locked == false then
			-- get combobox with device names
			local devList = enumDevices(value.allowed, value.device)
			devList.action = function(self, text, item, state)
				if state == 1 then
					local deviceDef = Devices[text]
					customData:mount(key,deviceDef)
				end
			end
			table.insert(list,iup.hbox
			{
				iup.label{title = key, size = "100x"}, devList
			})
		end
	end
	local result = iup.dialog
	{
		iup.hbox
		{
			iup.vbox(list),
			expand="HORIZONTAL",
			gap = 2,
		},title="Customize"
	}
	result:show()
	return result
end
-- создаёт игрока и цепляет управление к нему
function createPlayer(pnum)
	debugTrace('Creating player')
	takeControl(createUnit('tank',{-200,-200,5},{0,1,0},pnum))
end
-- отцепляемся управление от игрока
function resetPlayer()
	player=nil
	manualMover.mover=nil
end

DeadListeners = {}

view = CreateCamera()

player = nil

---------------------------------------------
-- Scene manipulation

testScenes = { dictionary = {} }

MakeComplexType("SceneObject")
{	
	MakeMember("type", Types.string),
	MakeMember("pose", Types.Pose),	
}
-- LevelDesc description
MakeComplexType("Scene")
{	
	MakeMember("name", Types.string),
	MakeMember("image", Types.FileName),
	MakeMember("solid", Types.bool),
	MakeMember("size", Types.float),
	--MakeMember("layerNavpoints", Types.Pose, "list"),
	--MakeMember("layerObjects", Types.SceneObject, "list"),
	--[[save_custom = function(self, value, offset)
		local empty = true
		local result = "\n" .. offset .. "{\n"
		for _, object in spairs(value.layerObjects) do
			empty = false
			result = offset .. "\t" .. Types.SceneObject:save(object, offset .. "\t") .. ",\n"
		end
		if empty then
			return "{}"
		else
			return result .. offset .. "}"
		end
	end,]]
}

ConstructLevel = SoftObject("Scene")
-- register test scene
function Scene(source)
	--print("creating scene from")
	--print(dumpsimple(source))
	local default =
	{
		name = "untitled",
		size = 5,
		solid = false,
		objects = {},					-- objects list
		-- called when we load scene
		load = function(self) 
			print("loading scene")
			if self.layerObjects then
				for _, value in ipairs(self.layerObjects) do
					print("creating object")
					local pose = value.pose
					self:createUnit(value.type,{ pose.x, pose.y, pose.orientation}, PlayerID.player)
				end
			end
		end,		
		unload = function(self)
			print("Scene:unload") 
			self:control(nil) 
		end,	-- called when we unload scene
		onControl = function(self) end,	-- called when some input occurs
		onMouse = function(self, key, eventType, x, y, object) 
			print("Scene::onMouse")
			if eventType == 1 and self.controlled ~= nil and self.controlled:getController() ~= nil then
				self.controlled:getController():clear()
				self.controlled:getController():cmdMoveTo(x,y,0)
			end
		end,

		update = function(self,dt) end,
		onUnitDie = function(self, unit) end,	-- called when someone dies

		createUnit = function(self, defName, pose, player)
			local angle = math.rad(pose[3])
			local dir = { math.cos(angle), math.sin(angle), 0 }
			local tank = createUnit(defName,{ pose[1], pose[2], 0}, dir, player, TiGa.createPerceptionAI)
			table.insert(self.objects, tank)
			--self.objects[tank] = true
			return tank
		end,

		nextUnit = function(self, unit)
			local iterator, object = next(self.objects, self.iterator)
			self.iterator = iterator
			if iterator == nil then 
				print("nextUnit> nobody is found - rewind to begin")
				iterator = next(self.objects) 
			end
			return object
		end,

		control = function(self, unit)
			if self.controlled~= nil then
				world:clearEffects(self.controlled)
				self.controlled = nil
			end	
			if TiGa.toUnit(unit)~=nil then
				self.controlled = TiGa.toUnit(unit)
				self.controlled:getController():clear()
				--self.controlled:enableAI(false)
				local effect = world:attachEffect(self.controlled, Models.selection)
				if effect then
					effect:start()
				end
			else			
			end
		end,
	}
	local scene = mergeTables(source,default)
	table.insert(testScenes, scene)
	testScenes.dictionary[ scene.name ] = scene
	core:registerTestScene(scene.name)
	print("Scene " .. scene.name .. " is registered")
	return ConstructLevel(source)
end

function LoadSceneByName(sceneName)
	local scene = testScenes.dictionary[sceneName]
	if scene == nil then
		print("Invalid scene ".. sceneName)
	else
		testScenes:load(scene)
	end
end

function AllObjects()
	local it = world.gameObjects:objectsBegin()
	local finish = world.gameObjects:objectsEnd()	
	return function()
		local old_it = it
		if it ~= finish then
			it = it:next()
			return old_it:getObject()
		else
			return nil
		end
	end
end

function AllUnits()	
	local finish = world.gameObjects:objectsEnd()	

	local find_unit = function(it)
		if it == finish then
			return nil
		else
			local unit = TiGa.toUnit(it:getObject())
			if unit then
				return it,unit
			else
				return find_unit(it:next())
			end
		end
	end

	local it,unit = find_unit(world.gameObjects:objectsBegin())
	
	return function()		
		if it == nil then
			return nil
		elseif it ~= finish then
			local object = unit
			it,unit = find_unit(it:next())
			return object
		else
			return nil
		end
	end
end

-- load scene
testScenes.load = function(self,level)
	if activeScene then
		activeScene:unload()
		activeScene = nil
	end
	if level then
		print("testScenes.load() : reset world state")
		self:reset()
		print("testScenes.load() : load new scene")
		if level.size == nil or level.size < 100 then
			level.size = 100
		end
		world:initMap(level.image, level.size/1000, level.solid,TiGa.Pose(0,0,0,-10))
		level:load()
		print("testScenes.load() : scene loaded")
		activeScene = level
		print("testScenes.load() : pause world")
		WorldPause()
		print("testScenes.load() : finished")
	else
		print("testScenes.load() : empty scene specified")
	end
end

-- reset scene
testScenes.reset = function(self)
	world.gameObjects:removeAllObjects()
	world:getLevel():clear()
	view:reset()
	onMouseMove = nil
	onMouseClick = nil
	player = nil
	activeScene = nil
end

------------------------------------------------------
-- events called from native World

-- called externally on every engine update
onUpdate = function(delta_t)
	view:update(delta_t)
end
-- called externally on every world update
onUpdateWorld = function(delta_t)
	if activeScene then
		activeScene:update(delta_t)
	end
end
-- called externally when any unit dies
onUnitDie = function(unit)
	debugTrace('some unit dies')
	--[[
	debugTrace(type(unit))
	debugTrace(type(enemy))
	]]--

	if activeScene then
		activeScene:onUnitDie(unit)
	end
	for object, _ in pairs(DeadListeners) do
		object:onUnitDie(unit)
	end
	--[[
	TiGa.testGameObject(unit)
	if isEnemy(TiGa.toUnit(unit)) then
		debugTrace('- respawning enemy')
		createEnemy()
	end
	if isPlayer(TiGa.toUnit(unit)) then
		debugTrace('- respawning player')
		player=nil
		createPlayer(6)
	end]]--
end

function ViewControl( key, eventType, x, y, wheel )
	if eventType ~= 0 then	
		local delta = eventType * 50
		local deltaA = eventType * 15
		local deltaS = eventType * 5
		
		if key == TiGa.HGEK_LEFT then
			view.vx = view.vx - delta
		end

		if key == TiGa.HGEK_RIGHT then
			view.vx = view.vx + delta
		end

		if key == TiGa.HGEK_UP then
			view.vy = view.vy - delta
		end

		if key == TiGa.HGEK_DOWN then
			view.vy = view.vy + delta
		end

		if key == TiGa.HGEK_PGUP then
			view.va = view.va - deltaA
		end

		if key == TiGa.HGEK_PGDN then
			view.va = view.va + deltaA
		end

		if key == TiGa.HGEK_SUBTRACT  or key == TiGa.HGEK_MINUS then
			view.vs = view.vs - deltaS
		end

		if key == TiGa.HGEK_ADD or key == TiGa.HGEK_EQUALS then
			view.vs = view.vs + deltaS
		end

		if key == TiGa.HGEK_I and eventType == 1 then
			print("View "..view.x .. "," .. view.y .. "," .. view.scale)
		end
		--[[
		if key == TiGa.HGEK_SPACE and eventType == 1 then
			view:goto(0, 0, 9, 2)
		end]]
	end
	view:update(0)
end

function SafeCall(fn, ...)
	if fn then
		return fn
	else
		return function() end
	end
end

ControlModes = 
{
	EditScene = function(key, eventType,x,y,wheel)
		--print("onControl - EditScene")
		local worldVec = {0.0,0.0}

		ViewControl(key, eventType, x, y, wheel )
		-- world coorditanes
		local wx = world.cursor.worldPos.x
		local wy = world.cursor.worldPos.y
		-- change cursor if we point to some object
		if world.cursor.object ~= nil then			
			world:setCursor(cursor_object)
		else
			world:setCursor(cursor_empty)
		end

		if eventType ~= 0 and (key == TiGa.HGEK_LBUTTON or key == TiGa.HGEK_RBUTTON) then
			SafeCall(onMouseClick)(key,world.cursor.worldPos.x,world.cursor.worldPos.y)			
		end
		
		if activeMode then			
			if eventType == 1 then
				activeMode:onMouseDown( key, wx, wy )
			elseif eventType == -1 then
				activeMode:onMouseUp( key, wx, wy )
			elseif eventType == 0 then
				activeMode:onMouseMove(wx, wy)
			end			
		end
	end,

	RunScene = function(key,eventType,x,y,wheel)
		--print("onControl - RunScene")
		ViewControl(key, eventType, x, y, wheel )

		if key == TiGa.HGEK_F2 and down == true then
			print("choose next object")
			activeScene:control(activeScene:nextUnit(self.controlled))
		end

		if world.cursor.object ~= nil then
			world:setCursor(cursor_object)
		else
			world:setCursor(cursor_empty)
		end

		if eventType and (key == TiGa.HGEK_LBUTTON or key == TiGa.HGEK_RBUTTON) then
			SafeCall(onMouseClick)(key,world.cursor.worldPos.x,world.cursor.worldPos.y)
		end

		if eventType and (key == TiGa.HGEK_LBUTTON or key == TiGa.HGEK_RBUTTON) then
			if activeScene then
				activeScene:onMouse( key, eventType, world.cursor.worldPos.x, world.cursor.worldPos.y, world.cursor.object )
			end
		end

		if player~=nil and player.controller~=nil then
			--debugTrace("blablabla2")
			local down = false
			if eventType == 1 then down = true end
			player.controller:onControl(key,down,world.cursor.worldPos.x,world.cursor.worldPos.y,wheel)
		end
	end,
	-- called externally when some input occurs
	Game = function(key,eventType,x,y,wheel)
		--print("onControl - Game")

		ViewControl(key, eventType, x, y, wheel )

		if key == TiGa.HGEK_F2 and eventType == 1 then
			print("choose next object")
			activeScene:control(activeScene:nextUnit(activeScene.controlled))
		end

		if world.cursor.object ~= nil then
			world:setCursor(cursor_object)
		else
			world:setCursor(cursor_empty)
		end

		if eventType == 1 and (key == TiGa.HGEK_LBUTTON or key == TiGa.HGEK_RBUTTON) then
			SafeCall(onMouseClick)(key,world.cursor.worldPos.x,world.cursor.worldPos.y)
		end

		if eventType == 1 and (key == TiGa.HGEK_LBUTTON or key == TiGa.HGEK_RBUTTON) then
			if activeScene then
				activeScene:onMouse( key, eventType, world.cursor.worldPos.x, world.cursor.worldPos.y, world.cursor.object )
			end
		end		

		if player~=nil and player.controller~=nil then
			--print("player control")
			local down = false
			if eventType == 1 then down = true end
			player.controller:onControl(key,down,world.cursor.worldPos.x,world.cursor.worldPos.y,wheel)
		end
	end,
}

onControl = ControlModes.Game

-- called when the World is destroyed
onGameFinish=function(self)
end

------------------------------------------------------
-- create button list for all existing scenes
function EnumScenes()
	local items = {}
	for key, level in ipairs(testScenes) do
		local item = iup.button
		{
			size = "130x",
			title=level.name,
			expand="HORIZONTAL",
			action=function(self)
				testScenes:load(level)
				return iup.DEFAULT
			end
		}
		table.insert(items,item)
	end
	return iup.vbox(items)
end

------------------------------------------------------
-- direct control over unit
function takeControl(unit)
	print("takeControl()")
	if player~= nil then
		world:clearEffects(player)
		player = nil
	end	
	if unit~=nil then
		player = TiGa.toUnit(unit)
		player:enableAI(false)
		local selection = Models.selection
		print("takeControl() - show selection")
		local effect = world:attachEffect(player, selection)		
		if effect then
			local size = player:getSphereSize()
			effect:setScale(size)
			effect:start()
		end
	else		
		print("takeControl() - unit is nil")	
	end
	print("takeControl() - finished")
end

function LoadDefaultLevel()
	cursor_object = Models.cursor_red
	cursor_empty = Models.cursor_green

	world:setCursor( cursor_empty )
	-- define default scene
	if emptyScene == nil then
		emptyScene = Scene{	name = "Empty" }
	end
	-- make default scene as active
	testScenes:load(emptyScene)
	return emptyScene
end

exec("./script/scenes.lua")
