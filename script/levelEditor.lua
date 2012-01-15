-- levelEditor.lua
-- Contains ui definition for level editor

OpenLevel = function()
	local file, status = DlgOpenFile("Open Level", ".", "Level|*.lvl")
	--local file, status = iup.GetFile(".\*.lvl")
	local result = 0
	if status ~= -1 then
		print("LoadLevel ".. file .. " was selected")	
		world:loadLevel(file)
		local lvl = world:getLevel()
		activeScene.size = lvl.cellSize
		activeScene.image = lvl.mapName
		activeScene.solid = lvl:isSolid()
		print("LoadLevel: level script executed")	
	else
		print("LoadLevel: choice canceled")	
	end	
	print("LoadLevel: finished")
	return activeScene
end

function TestObjects()
	for unit in AllUnits() do		
		local pose = unit:getPose()
		local objectDef = unit:getDefinition()			
		print(objectDef.name .. " id = " .. unit:id() .. " found with pose {" .. pose.x .. "," .. pose.y .. "," .. pose.orientation .. "," .. pose.z .. "}")		
	end
end

SaveLevel = function()
	-- 1. select file
	local file, status = iup.GetFile(".\*.lvl")
	if status ~= -1 then
		activeScene.layerObjects = {}
		world:saveLevel(file)
	else
		print("SaveLevel: choice canceled")
	end
end

function LevelProperties()		
	local revealer = Editor:RevealObject(activeScene, "SceneDesc")
	local result = 
	{	
		updateUp = function(self)
			print("level desc has been changed")
			self.wndButtonApply.active = "yes"
		end,		
	}
	result.wndButtonApply = iup.button
	{
		title = "Apply",
		active = "no",
		expand = "yes",
		action = function(self)
			testScenes:load(activeScene)
			self.active = "no"
			result.control:destroy()
			return iup.IGNORE
		end,
	}
	result.control = iup.dialog
	{
		title = "LevelDesc",		
		DEFAULTENTER = "Apply",
		DEFAULTESC = "Cancel",
		topmost = "yes",
		active = "yes",
		
		iup.vbox
		{
			revealer.control,
			result.wndButtonApply,			
		}
	}
	revealer.parent = result
	result.control:popup()		
	return result
end

function drawNavpoints()
	--[[if level.layerNavpoints then
		for _, value in pairs(level.layerNavpoints) do		
			navpointIcon:render(value)
		end
	end]]
end

function onRenderObjects()
	drawNavpoints()
end



-- setup current mode
function SetActiveMode(mode)
	local newMode = {}
	if mode then
		newMode = mode
	else
		newMode = ModeDefault()
	end
	if activeMode then
		activeMode:onExit()
	end
	activeMode = newMode
	SetActionStateName(activeMode.name)	
end
-- default mode
function ModeDefault()
	function ShowContextMenuObject(object)
		contextMenuObjectSelected = object
		contextMenuObject:popup(iup.MOUSEPOS,iup.MOUSEPOS)
	end	
	return
	{
		name = "no action" ,
		typeName = typeName,
		onExit = function(self)
			self:select(nil)
		end,
		select = function(self, selection)
			-- if we do not change selection, then exit
			if selection == self.selection then return end
			-- if we select new object
			if selection then
				-- reset last selected object
				self:select(nil)
				-- setup new selected object
				self.selection = selection
				local effect = world:attachEffect(selection, Models.selection)
				if effect then
					
					local size = selection:getSphereSize() * 2
					effect:setScale(size)
		
					effect:start()
				else
					print("cannot attach selection effect")
				end			
			else
				world:clearEffects(self.selection)
				self.selection = nil
			end
		end,
		onMouseDown = function( self, key, x, y )
			local selection = TiGa.toUnit(world.cursor.object)			
			if key == TiGa.HGEK_LBUTTON and selection then				
				self:select(selection)
			end
			if key == TiGa.HGEK_RBUTTON then
				if self.selection ~= nil then
					IssueOrder(self.selection, x, y, world.cursor.object)
					--self.selection:getController():cmdMoveTo(x,y,0)
				elseif world.cursor.object ~= nil then
					ShowContextMenuObject(world.cursor.object)
				end
			end
		end,
		onMouseMove = function(self, x, y) end,
		onMouseUp = function( self, key , x, y ) end,
	}
end
-- creating unit
function ModeCreateUnit(typeName)
	return
	{
		name = "Create " .. typeName,
		typeName = typeName,
		pressed = false,
		-- called when we exit this mode
		onExit = function(self)	end,
		onMouseDown = function( self, key, x, y )
			if key == TiGa.HGEK_LBUTTON then
				self.created = activeScene:createUnit(self.typeName, {x,y,0}, PlayerID.NPC,TiGa.createPerceptionAI)
				self.pressed = true
			end
			if key == TiGa.HGEK_RBUTTON then
				self.created = nil
				SetActiveMode(nil)
			end
		end,

		onMouseMove = function( self, x, y)
			local object = self.created
			if self.pressed and object then
				local pose = object:getPose()
				object:setDirection(x - pose.x, y - pose.y)
			end
		end,

		onMouseUp = function( self, key, x, y ) 
			if key == TiGa.HGEK_LBUTTON then
				local object = self.created
				if object then
					local pose = object:getPose()
					object:setDirection(x - pose.x, y - pose.y)
				end
				self.pressed = false
				self.created = nil
			end
		end,
	}
end

function StartLevelEditor()
	mainMenu = iup.menu
	{
		iup.submenu
		{
			iup.menu
			{				
				iup.item
				{
					title = "New",
					action = function(self)		
						LoadDefaultLevel()					
						return iup.DEFAULT
					end
				},	
				iup.item
				{
					title = "Open",
					action = function(self)						
						OpenLevel()
						return iup.DEFAULT
					end
				},					
				iup.item
				{
					title="Save",
					action=function(self)
						SaveLevel(level)
						return iup.DEFAULT
					end
				},
				iup.item
				{
					title="Properties",
					action=function(self)		
						LevelProperties(activeLevel)
						return iup.DEFAULT
					end
				},
				iup.item
				{

					title = "Exit",
					action = function(self)
						iup.ExitLoop()
						return iup.DEFAULT
					end
				},		
			},
			title = "File",
		},
	}

	contextMenuObject = iup.menu
	{
		iup.item
		{
			title = "Remove",
			action = function(self)
				print("remove")
				if contextMenuObjectSelected then
					TiGa.DestroyObject(contextMenuObjectSelected)
					contextMenuObjectSelected = nil					
				else
					print("object is nil")
				end					
			end,
		},
		iup.item
		{
			title = "Clear orders",
			action = function(self)
				local unit = TiGa.toUnit(contextMenuObjectSelected)
				if unit then
					local controller = unit:getController()
					if controller then
						controller:clear()
					end
				end
				return iup.DEFAULT
			end,
		},
		iup.item
		{
			title = "Owner",
			action = function(self)
				return iup.DEFAULT
			end,
		},
		menuclose_cb = function(self)
			print("close menu")			
		end,
	}

	local wndSimulationState = iup.label
	{
		--readonly = "yes",
		PADDING = "3",
		title = "empty",
	}
	
	local wndActionState = iup.label
	{
		padding = "3",
		title = "no action",
	}

	function SetSimulationStateName(name)
		if wndSimulationState then
			wndSimulationState.title = name
		end
	end

	function SetActionStateName(name)
		if wndActionState then
			wndActionState.title = name
		end
	end

	function WorldRun()
		onControl = ControlModes.RunScene
		SetSimulationStateName("Run")
		core:worldPause(false)
	end

	function WorldPause()
		onControl = ControlModes.EditScene
		SetSimulationStateName("Paused")
		core:worldPause(true)
		SetActiveMode(nil)
	end

	function WorldReset()
		testScenes:load(activeScene)
		WorldRun()
	end

	local toolbox = iup.frame
	{
		title = "toolbox",
		expand = "no",		
	}

	local modeBox = iup.frame
	{
		title = "settings",
		expand = "no",
	}
	local wndStatusBar = iup.hbox
	{
		wndSimulationState,
		wndActionState
	}

	-- we attach HGE to this control
	local hgeContainer = iup.glcanvas
	{ 		
		buffer = "DOUBLE",
		DEPTH_SIZE = "16",
		expand = "YES",
		rastersize = "640x480",
		DRAWSIZE = "640x480",
		--action = function() end,
	}

	function GetLevelEditorWnd()
		return levelEditor
	end

	-- this is main window for level editor
	levelEditor = iup.dialog
	{		
		menu = mainMenu,
		maxbox = "YES",
		minbox = "YES",
		size = "FULLxFULL",
		iup.hbox
		{
			iup.vbox
			{
				modeBox,
				toolbox,
			},
			iup.vbox
			{
				hgeContainer,
				wndStatusBar,
			},
		},title="Main menu",
		close_cb = function(self)
			return iup.CLOSE
		end,
	} 

	trace("main window is created")

	levelEditor:showxy(iup.LEFT,iup.CENTER)

	core:startHGEChild(convertHandle(hgeContainer.wid))
	
	StartWorldEditor()

	timer1 = iup.timer{time = 66}


	function timer1:action_cb()
	  return iup.DEFAULT
	end
	-- can only be set after the time is created
	timer1.run = "YES"

	function InsertControl(container, control)
		iup.Append(container, control)
		iup.Map(control)
		iup.Refresh(container)
	end

	-- enum all levels
	function GenerateLevelChoise(name)
		local menu = {}
		for _, scene in ipairs(testScenes) do
			local item = iup.item
			{
				title = scene.name,
				action = function(self)	
					testScenes:load(scene)						
					return iup.DEFAULT
				end,
			}
			table.insert(menu, item)
		end

		return iup.submenu
		{
			iup.menu(menu),
			title = name,
		}
	end
	local buttonSize = "100x20"
	function GenerateObjectsChoise(group)
		local result = {}		
		if PackHard.objects[group] then
			for key, value in pairs(PackHard.objects[group]) do
				local typename = key
				table.insert(result, iup.button
				{
					title = typename,
					size = buttonSize,
					action = function(self)
						SetActiveMode(ModeCreateUnit(typename))
					end,
				})
			end
		end
		return iup.vbox{HOMOGENEOUS="yes", NORMALIZESIZE = "yes", expand = "yes", unpack(result)}
	end
	local cmdMove = iup.toggle
	{ --[[title = title, size = buttonSize,]] 
		action = function(self)
			return iup.DEFAULT
		end, 
		image = "./data/toolbar/move.bmp",
	}	
	local cmdPatrol = iup.toggle
	{
		action = function(self)
			return iup.DEFAULT
		end,
		image = "./data/toolbar/patrol.bmp",
	}
	local ActiveCmd = iup.radio
	{
		iup.radio
		{
			iup.hbox
			{			
				cmdMove,
				cmdPatrol,
			},
			value = cmdMove,			
		}
	}

	function IssueOrder(unit, x, y, target)
		local controller = unit:getController()
		if ActiveCmd.value == cmdMove then
			controller:cmdMoveTo(x,y,0)
		elseif ActiveCmd.value == cmdPatrol then
			controller:cmdPatrol(x,y,0)
		end
	end

	local Action = function(title, action, image) return iup.button{ --[[title = title, size = buttonSize,]] action = action, image = image } end
	InsertControl(mainMenu, GenerateLevelChoise("Levels"))
	InsertControl(toolbox, GenerateObjectsChoise("Units"))
	InsertControl(modeBox, iup.vbox
	{		
		EXPANDCHILDREN = "YES",
		iup.hbox
		{
			Action("Run", WorldRun,"IUP_MediaPlay"),
			Action("Pause", WorldPause, "IUP_MediaPause"),
			Action("Reset", WorldReset, "IUP_MediaGotoBegin"),
			Action("Settings", LevelProperties, "IUP_FileProperties"),
		},
		iup.toggle
		{
			title = "draw tasks",
			value = bool2iup(world.draw.drawTasks),
			action = function(self, value)
				world.draw.drawTasks = iup2bool(value)
			end,
		},
		iup.toggle
		{						
			title = "draw devices",
			value = bool2iup(world.draw.drawDevices),
			action = function(self, value)
				world.draw.drawDevices = iup2bool(value)
			end,
		},
		iup.toggle
		{
			title = "draw dynamics",
			value = bool2iup(world.draw.drawDynamics),
			action = function(self, value)
				world.draw.drawDynamics = iup2bool(value)
			end,
		},
		ActiveCmd,
	})
	return levelEditor
end