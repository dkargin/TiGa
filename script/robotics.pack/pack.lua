depends("basic")

include("models.lua")

Devices
{
	mover01 = MoverVehicleDef
	{
		maxVelocity=TiGa.vec2f(2,5),
		smooth = false,
		adjacency = 24,
		bounds = false,
		heuristic = true,
		kinematic = false,
		--acceleration={100,4},
		moveBack=true,
		--brakeForce={1,1}
	},
	mover02 = MoverVehicleDef
	{
		maxVelocity = TiGa.vec2f(2,40),
		acceleration = TiGa.vec2f(100,100),
		kinematic = true,
		moveBack = true,
		--brakeForce={1,1}
	},
	mover03 = MoverVehicleDef
	{
		maxVelocity = TiGa.vec2f(2,10),
		acceleration = TiGa.vec2f(100,20),
		kinematic = true,
		moveBack = true,
		--brakeForce={1,1}
	},
	mover_ch01 = MoverCharacterDef
	{
		kinematic = true,
		speedLinear = 100,
		speedAngular = 20,
	},
	mover_ch02 = MoverCharacterDef
	{
		kinematic = false,
		speedLinear = 100,
		speedAngular = 20,
		thrust = 100000,
		torque = 400,
	},
	perception360 = PerceptionDef
	{
		fov=360,
		distance=400,
	},
	perception_far = PerceptionDef
	{
		fov=360,
		distance=1700,
	},
	perception240 = PerceptionDef
	{
		fov=240,
		distance=600,
	},
	targeting01 = TargetingSystemDef{},
}

Units
{
	tree = UnitDef
	{
		health = 100,
		fxIdle = Models.tree,
		body = BodyDesc{ solidSphere(0.5,0.0), frozen = true, friction = 0.5},
	},
	asteroid = UnitDef
	{
		health = 100,
		fxIdle = Models.asteroid,
		body = BodyDesc{ solidSphere(0.5,0.1), friction = 0.5},
	},
	box = UnitDef
	{
		health = 100,
		fxIdle = Models.box,
		body = BodyDesc{ solidBox(1,1,0.1) },
	},	
	robot = UnitDef
	{
		health = 100,
		fxIdle = Models.wbr914,
		body = BodyDesc{ solidSphere(0.5,0.1) },
		_mounts=		-- describe mounting points. Also can define attached devices right here
		{
			intern01=_Mount
			{
				allowed = { Slots.internal, Slots.engine },
				device = Devices.mover01,
			},--_Mount
			watch01=_Mount
			{
				allowed = { Slots.internal, Slots.misc },
				device = Devices.perception_far,
			},--_Mount
		},--_mounts
	},
	robot2 = UnitDef
	{
		health = 100,
		fxIdle = Models.wbr914,
		body = BodyDesc{ solidSphere(0.5,0.1) },
		_mounts=		-- describe mounting points. Also can define attached devices right here
		{
			intern01=_Mount
			{
				allowed = { Slots.internal, Slots.engine },
				device = Devices.mover03,
			},--_Mount
			watch01=_Mount
			{
				allowed = { Slots.internal, Slots.engine },
				device = Devices.perception240,
			},--_Mount
		},--_mounts
	},
	pedestrian = UnitDef
	{
		health = 100,
		fxIdle = Models.pedestrian,
		body = BodyDesc{ solidSphere(0.5,0.1) },
		_mounts=		-- describe mounting points. Also can define attached devices right here
		{
			intern01 = _Mount
			{
				allowed = { Slots.internal, Slots.engine },
				device = Devices.mover_ch01,
			},--_Mount
			watch01 = _Mount
			{
				allowed = { Slots.internal, Slots.misc },
				device = Devices.perception360,
			},--_Mount
		},--_mounts
	},
}
