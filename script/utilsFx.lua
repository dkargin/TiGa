-- utils.lua
-- Core script
-- File contains FxObjects tools
Models = {}

function fxRegisterObject(object)
	--table.insert(Models,object)
	return object
end

-- read parameters from init={pose={x,y,a},scale} to FX object
function fxInit(object,init)
	local init = mergeTables(init,{pose = Pose{0,0,0,0},scale=1.0})
	object.scale = init.scale
	object.pose = init.pose
	return fxRegisterObject(object)
end

-- create array of FxEffects
function fxArray(source)
	local manager = world.gameObjects:getFxManager()
	local holder = manager:fxHolder()
	for key,object in ipairs(source) do
		holder:attach(object)
	end
	return holder:base()
end

function fxSound(source)
	local manager = world.gameObjects:getFxManager()
	local object = manager:fxSound(source.file)
	return object
end
--
-- create sprite and write basic params
function fxSprite(init)
	local data = mergeTables(init,{file = "data/zazaka.png",rect={0,0,0,0},blend = TiGa.binOR(TiGa.COLORMUL,TiGa.ALPHABLEND,TiGa.ZWRITE)})
	local manager = world.gameObjects:getFxManager()
	local object = manager:fxSprite(data.file,data.rect[1],data.rect[2],data.rect[3],data.rect[4])
	local typedef = getmetatable(object)
	print(type(typedef))
	object:setBlendMode(init.blend)
	return fxInit(object,init):base()
end

-- create animation using specific texture region
function fxAnimationRC(init)
	--debugTrace('fxAnimationRC')
	local manager= world.gameObjects:getFxManager()
	init = mergeTables(init,{frame={0,0},rect={0,0,0,0},fps=25,blend=TiGa.binOR(TiGa.COLORMUL,TiGa.ALPHAADD,TiGa.ZWRITE),mode=TiGa.Hold})
	--(const char *texture,float rect[4],int frameWidth,int frameHeight,float fps,AnimMode mode);
	local rect=TiGa.Rect(init.rect[1],init.rect[2],init.rect[3],init.rect[4])
	object = manager:createAnimation(init.file,rect,init.frame[1],init.frame[2],init.fps,init.mode)
	object:setBlendMode(init.blend)

	if type(init.realSize) == "table" then
		object:setSize(init.realSize[1], init.realSize[2],true)
	end

	if type(init.realSize) == "number" then
		local size = init.realSize
		local w = object:getWidth()
		local h = object:getHeight()
		object:setSize(size, size * h / w, true)
	end

	return fxInit(object,init):base()
end

-- create animation using all texture
function fxAnimation2(init)
	--debugTrace('fxAnimation2')
	local manager = world.gameObjects:getFxManager()
	--debugTrace('fxAnimation2-scale='..init.scale)
	local init = mergeTables(init,{frame={0,0},fps=25,blend=TiGa.binOR(TiGa.COLORMUL,TiGa.ALPHABLEND,TiGa.ZWRITE),mode=TiGa.Hold})
	--debugTrace('fxAnimation2-scale='..init.scale)
	local object = manager:createAnimationFull(init.file,init.frame[1],init.frame[2],init.fps,init.mode)
	object:setBlendMode(init.blend)

	if type(init.realSize) == "table" then
		object:setSize(init.realSize[1], init.realSize[2],true)
	end

	if type(init.realSize) == "number" then
		local size = init.realSize
		local w = object:getWidth()
		local h = object:getHeight()
		object:setSize(size, size * h / w, true)
	end
	return fxInit(object,init):base()
end

--------------------------------------------------------------
-- hgeColor constructor
function hgeColor(r,g,b,a)
	local color = mergeTables({r,g,b,a},{0,0,0,1})
	return TiGa.hgeColorRGB(color[1],color[2],color[3],color[4])
end

--------------------------------------------------------------
-- particle system info constructor
function hgeParticleSystemInfo(source)
	local default = 
	{
		nEmission = 10,
		fLifetime = 1.0,
		fParticleLifeMin = 1.0,
		fParticleLifeMax = 1.0,

		fDirection = 0.0,
		fSpread = 360,
		bRelative = false,

		fSpeedMin = 100,
		fSpeedMax = 100,

		fGravityMin = 0,
		fGravityMax = 0,

		fRadialAccelMin = 0,
		fRadialAccelMax = 0,

		fTangentialAccelMin = 0,
		fTangentialAccelMax = 0,

		fSizeStart = 10,
		fSizeEnd = 10,
		fSizeVar = 0,

		fSpinStart = 0,
		fSpinEnd = 0,
		fSpinVar = 0,

		colColorStart = hgeColor(1,1,1),
		colColorEnd = hgeColor(1,1,1),
		fColorVar = 0,
		fAlphaVar = 0,
	}
	local result = TiGa.hgeParticleSystemInfo()
	SWIG_assign(result,mergeTables(source,default))
	return result
end

--------------------------------------------------------------
-- particle system constructor
function fxParticles(source)
	local manager = world.gameObjects:getFxManager()
	local object = manager:fxParticles(source.sprite, hgeParticleSystemInfo(source))
	return fxInit(object,source)
end

--------------------------------------------------------------
-- create fx object and add to model lib
-- source can be a table, or a single object
function ModelLib(source)
	if type(source) == "table" then
		for key, val in pairs(source) do
			Models[key] = val
		end
	else
		table.insert(Models,source)
	end
end