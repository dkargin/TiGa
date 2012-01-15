-- models.lua
--
Models
{
	vehicle = fxAnimation2
	{	
		file="data/units/4wheeled.png",
		scale = 1,
		realSize = 0.4,
	},	
	projectile01 = fxAnimation2
	{
		file="data/projectile01.png",
		realSize = 0.1,
	},
	smoke01 = fxAnimation2
	{
		file = "data/Effects/Smoke_01.png",
		rect = {0,0,128,128},
		frame = {64,64},
		fps = 30,
		mode = TiGa.Loop,
		scale = 0.5,
	},
	smoke02 = fxAnimation2
	{
		file = "data/Effects/Smoke_02.png",
		rect = {0,0,128,128},
		frame = {64,64},
		fps = 30,
		mode = TiGa.Loop,
		realSize = 0.25,
	},
	pulse01 = fxAnimationRC
	{
		file = "data/Effects/Pulse_03.png",
		rect = {0,0,128,128},
		frame = {32,32},
		fps = 16,
		mode = TiGa.Loop,
		realSize = 0.25,
	},
	pulse02 = fxAnimationRC
	{
		file = "data/Effects/Pulse_03.png",
		rect = {128,0,128,128},
		frame = {32,32},
		fps = 16,
		mode = TiGa.Loop,
		realSize = 0.25,
	},
	pulse03 = fxAnimationRC
	{
		file = "data/Effects/Pulse_03.png",
		rect = {0,128,128,128},
		frame = {32,32},
		fps = 16,
		mode = TiGa.Loop,
		realSize = 0.25,
	},
	pulse04 = fxAnimationRC
	{
		file = "data/Effects/Pulse_03.png",
		rect = {128,128,128,128},
		frame = {32,32},
		fps = 30,
		mode = TiGa.Loop,
		realSize = 0.25,
	},
	explosion01 = fxAnimation2
	{
		file="data/effects/explosion_anim.png",
		frame={64,64},
		fps=20,
		mode=TiGa.Once,
		realSize = 0.25,
		blend = binOR(TiGa.COLORMUL,TiGa.ALPHAADD,TiGa.NOZWRITE),
	},
	explosion02 = fxArray
	{
		fxAnimation2
		{
			file="data/effects/explosion_anim.png",
			frame={64,64},
			fps=20,
			mode=TiGa.Once,
			realSize = 0.25,
			blend = binOR(TiGa.COLORMUL,TiGa.ALPHAADD,TiGa.NOZWRITE),
		},
		fxSound{ file = "data/sounds/cannon_hit01.wav"},
	},
	grass = fxAnimation2
	{
		--file="data/water_deep.png",
		file="data/grass_1024.jpg",
		frame={1024,1024},
		fps=20,
		mode=Once,
		scale=0.5,
	},
	box = fxAnimationRC
	{
		file="data/_box.PNG",
		fps=20,
		frame={128,128},
		mode=TiGa.Loop,
		blend = binOR(TiGa.COLORMUL,TiGa.ALPHABLEND, TiGa.NOZWRITE),
		scale=0.25,
	},
	pedestrian = fxSprite{ file = "data/pedestrian.png", pose = Pose(0,0,0) },	

	flare01 = fxAnimation2
	{
		--file="data/water_deep.png",
		file="data/Effects/weapon_flare.png",
		blend = binOR(TiGa.COLORMUL,TiGa.ALPHAADD, TiGa.NOZWRITE),
		frame={128,128},
		fps=20,
		mode=Once,
		scale=0.5,
	},

	particlesTest = fxParticles
	{
		sprite = fxSprite
		{
			file="data/zazaka.png",
			blend = binOR(TiGa.COLORMUL,TiGa.ALPHAADD, TiGa.NOZWRITE),
			scale=0.1,
		},
		nEmission = 100,
		fLifetime = 1000.0,
		fParticleLifeMin = 100.0,
		fParticleLifeMax = 100.0,

		fDirection = 0.0,
		fSpread = 360,
		bRelative = false,

		fSpeedMin = 50,
		fSpeedMax = 100,

		fGravityMin = 0,
		fGravityMax = 0,

		fRadialAccelMin = -10,
		fRadialAccelMax = -40,

		fTangentialAccelMin = 1,
		fTangentialAccelMax = 1,

		fSizeStart = 0.1,
		fSizeEnd = 0.1,
		fSizeVar = 0.2,

		fSpinStart = 0,
		fSpinEnd = 0,
		fSpinVar = 0,

		colColorStart = hgeColor(1,1,1),
		colColorEnd = hgeColor(0,0,0),
		fColorVar = 0.2,
		fAlphaVar = 0,
	},
	explosion_smoke = fxParticles
	{
		sprite = fxSprite
		{
			file="data/Effects/smoke.png",
			blend = binOR(TiGa.COLORMUL,TiGa.ALPHAADD, TiGa.NOZWRITE),
			scale=0.1,
		},
		nEmission = 100,
		fLifetime = 10.0,
		fParticleLifeMin = 1.0,
		fParticleLifeMax = 10.0,

		fDirection = 0.0,
		fSpread = 360,
		bRelative = false,

		fSpeedMin = 50,
		fSpeedMax = 100,

		fGravityMin = 0,
		fGravityMax = 0,

		fRadialAccelMin = -10,
		fRadialAccelMax = -40,

		fTangentialAccelMin = 1,
		fTangentialAccelMax = 1,

		fSizeStart = 0.1,
		fSizeEnd = 0.1,
		fSizeVar = 0.2,

		fSpinStart = 0,
		fSpinEnd = 0,
		fSpinVar = 0,

		colColorStart = hgeColor(1,1,1),
		colColorEnd = hgeColor(0,0,0),
		fColorVar = 0.2,
		fAlphaVar = 0,
	},
	smallTurret = fxAnimation2
	{
		file = "data/weapons/turret01.png",
		frame = {64,32},
		fps = 20,
		mode = TiGa.Once,
		realSize = 0.5,		
		pose = Pose{0.1,0},
	},
	mediumTurret = fxAnimation2
	{
		file="data/weapons/turret02.png",
		frame={128,64},
		fps=20,
		mode=TiGa.Once,
		realSize = 0.5,		
		pose = Pose{0.05 ,0},
	},
}