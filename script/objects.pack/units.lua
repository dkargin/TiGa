-- units.lua
--

Units
{	
	rocket01 = UnitDef
	{
		health = 10,
		fxIdle= Models.projectile01,
		body = BodyDesc
		{
			solidBox(0.10,0.1,0.01)
		},
		_mounts=		-- describe mounting points. Also can define attached devices right here
		{
			intern01=_Mount
			{
				allowed = { Slots.internal, Slots.engine },
				locked = true,
				device = Devices.thruster01,
				pose = Pose{0,0,0},
			},--_Mount
			watch01 = _Mount
			{
				allowed = { Slots.external, Slots.misc },
				locked = false,
				pose = Pose{0.10,0,0},
				device = Devices.perception_far,
			},--_Mount
		},--_mounts
	},	
	tank = UnitDef
	{
		health = 100,
		fxIdle = Models.vehicle,
		body = BodyDesc{ solidBox(0.4, 0.2 ,0.01) },
		_mounts=		-- describe mounting points. Also can define attached devices right here
		{
		--[[
			intern03 = _Mount
			{
				allowed = { Slots.internal, Slots.misc },
				locked = false,
				device = Devices.targeting01,
			},--_Mount]]
			hardpoint01=_Mount
			{
				allowed = { Slots.external, Slots.weapon },
				locked=false,
				pose = Pose{0,0,1},
				device=Devices.cannon, --Devices.cannon,
			},--_Mount
			intern01=_Mount
			{
				allowed = { Slots.internal, Slots.engine },
				locked=true,
				device=Devices.mover01,
			},--_Mount
--[[
			intern02 = _Mount
			{
				allowed = { Slots.internal, Slots.misc },
				locked = false,
				device = Devices.shield01,
			},--_Mount
]]--
			watch01=_Mount
			{
				allowed = { Slots.external, Slots.misc },
				locked=false,
				pose=Pose{10,0,0},
				device=Devices.perception360,
			},--_Mount
		},--_mounts
	},
	vehicle = UnitDef
	{
		health = 100,
		fxIdle = Models.vehicle,
		body = BodyDesc{ solidBox(0.4, 0.2, 0.01) },
		_mounts=		-- describe mounting points. Also can define attached devices right here
		{

			intern01=_Mount
			{
				allowed = { Slots.internal, Slots.engine },
				locked=true,
				device=Devices.mover02,
			},--_Mount

			watch01=_Mount
			{
				allowed = { Slots.external, Slots.misc },
				locked=false,
				pose = Pose{0.1, 0, 0},
				device=Devices.perception360,
			},--_Mount

		},--_mounts
	},
	mrRound = UnitDef
	{
		health = 100,
		fxIdle = Models.round,
		body = BodyDesc{ solidSphere(15,0.1) },
		_mounts=		-- describe mounting points. Also can define attached devices right here
		{
			hardpoint01 = _Mount
			{
				locked = false,
				device = Devices.laser,
			},--_Mount
			intern01 = _Mount
			{
				allowed = { Slots.internal, Slots.engine },
				locked = false,
				device = Devices.mover01,
			},--_Mount
			watch01 = _Mount
			{
				allowed = { Slots.internal, Slots.misc },
				locked = true,
				pose = Pose{10,0,0},
				device = Devices.perception360,
			},--_Mount
		},--_mounts
	},
}
