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
exec('./script/levelRules.lua')

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
	PackHard = loadPack('objects',true):activate()
	--PackHard = loadPack('robotics',true):activate()
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

	LoadDefaultLevel()

	worldStarted = true
end

function SavePackRegistry(stream)
	-- count all loaded packs
	local amount = 0
	for packName, pack in pairs(PacksRegistry) do
		amount = amount + 1
	end


end

function LoadPackRegistry(stream)
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

-- start world in editor mode
function StartWorldEditor()
	core:createWorld()
	world = TiGa.world

	print("StartWorldEditor: loading objects")	
	PackHard = loadPack('objects',true):activate()
	print("StartWorldEditor: loading finished")

	SetupCollisions()

	exec('./script/server.lua')

	LoadDefaultLevel()

	worldStarted = true
end

exec('./script/mainMenu.lua')
StartMainMenu()
