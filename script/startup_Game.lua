-- startup.lua
-- Core script
-- general initalisation

function exec(path)
	logger:line(-1,'executing '..path)	
	logger:levelInc()
	dofile(path)
	logger:levelDec()
	print("executing done")
end

function print(data)
	logger:line(-1,data)
end

core = TiGa["core"]

logger = TiGa.g_logger

exec('./script/utils.lua')
exec('./script/utilsTiGa.lua')

worldStarted = false

local function SetupCollisions()
	local drawMap = false
	world.drawDynamics = true

	-- init collision flags
	world:collisionsSet(TiGa.cgUnit,TiGa.cgUnit,true)
	world:collisionsSet(TiGa.cgUnit,TiGa.cgStaticLow,true)
	world:collisionsSet(TiGa.cgUnit,TiGa.cgStaticHigh,true)
	world:collisionsSet(TiGa.cgUnit,TiGa.cgBorder,true)

	world:collisionsSet(TiGa.cgFlying,TiGa.cgFlying,true)
	world:collisionsSet(TiGa.cgFlying,TiGa.cgProjectile,true)
	world:collisionsSet(TiGa.cgFlying,TiGa.cgStaticHigh,true)
	world:collisionsSet(TiGa.cgFlying,TiGa.cgBorder,true)

	world:collisionsSet(TiGa.cgProjectile,TiGa.cgProjectile,false)
	world:collisionsSet(TiGa.cgProjectile,TiGa.cgUnit,true)
	world:collisionsSet(TiGa.cgProjectile,TiGa.cgStaticHigh,true)
	world:collisionsSet(TiGa.cgProjectile,TiGa.cgFlying,true)
	world:collisionsSet(TiGa.cgProjectile,TiGa.cgShield,true)
	world:collisionsSet(TiGa.cgProjectile,TiGa.cgBorder,true)
end

-- start world in game mode
function StartWorld()
	core:createWorld()
	world = TiGa.world	

	print("StartWorld: loading objects")		
	PackHard = loadPack('basic',true):activate()
	print("StartWorld: loading finished")

	SetupCollisions()
	onGameInit=function()
		debugTrace('Events:onInit')
	end

	print("StartWorld: executing game rules script")
	
	if world.server then
		exec('./script/server.lua')
	else
		exec('./script/client.lua')
	end
	
	core:uiSetDefaultCursorEffect(Models.cursor_green)
	core:uiResetCursorEffect(0)
	
	worldStarted = true
end

-- called during World::SaveState()
function WorldSaveState(stream)
	
	TiGa.IO.writeString(stream,"blablabla")
end
-- called during World::LoadState()
function WorldLoadState(stream)
	local tmp = TiGa.IO.readString(stream)
	print("WorldLoadState :" .. tmp)
end

core:startHGE()

function WorldRun()
	onControl = ControlModes.Game				
	core:worldPause(false)
end

function WorldPause()
	onControl = ControlModes.Game				
	core:worldPause(true)				
end

function WorldReset()
	testScenes:load(activeScene)
	WorldRun()
end

StartWorld()

