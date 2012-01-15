-- devices.lua
--

Devices
{
	cannon = WeaponTurretDef
	{
		projectile = Projectiles.cannon_trail,
		--projectile = Projectiles.missile,
		velocity = 600,
		dimensions = 180,
		muzzleOffset = 0.35,
		timeShootDelay = 0.3,
		maxAmmo = 5,
		timeReload = 5,
		spread = 0,
		--fxShoot=fxAnimation{name="Turret01",scale=0.5},
		fxShoot = fxArray
		{	
			Models.smallTurret,
			fxSound{ file = "data/sounds/cannon_shoot01.wav" }
		}		
	},
	shotgun = WeaponTurretDef
	{
		projectile = Projectiles.cannon,
		weaponType = TiGa.WeaponDef_weaponProjectile,
		velocity = 600,
		dimensions = 180,
		muzzleOffset = 0.35,
		timeShootDelay = 0.3,
		maxAmmo = 5,
		timeReload = 5,
		spread = 5,
		burstSize = 10,
		burstDelay = 0.01,
		fxShoot = Models.smallTurret,
	},
	heavy_cannon = WeaponTurretDef
	{
		projectile = Projectiles.energy,
		velocity = 100,
		dimensions = 90,
		muzzleOffset = 0.35,
		timeShootDelay=1.3,
		maxAmmo=5,
		timeReload=5,
		burstSize = 2,
		spread=2,
		fxShoot = Models.mediumTurret,
		external = true,
	},
	laser = WeaponTurretDef
	{
		velocity=100,
		dimensions=180,
		muzzleOffset=0.05,
		timeShootDelay=0.3,
		timeShootDuration=0.2,
		maxAmmo=5,
		timeReload=5,
		spread=2,
		fxShoot = Models.smallTurret,
		external = true,
	},
	--[[mover01 = MoverVehicleDef
	{
		maxVelocity=TiGa.vec2f(60,5),
		smooth = false,
		adjacency = 24,
		bounds = false,
		heuristic = true,
		--acceleration={100,4},
		moveBack=true,
		--brakeForce={1,1}
	},
	mover02 = MoverVehicleDef
	{
		maxVelocity = TiGa.vec2f(1000,40),
		acceleration = TiGa.vec2f(10000,100),
		kinematic = true,
		moveBack = true,
		--brakeForce={1,1}
	},
	mover03 = MoverVehicleDef
	{
		maxVelocity = TiGa.vec2f(1000,400),
		acceleration = TiGa.vec2f(100000,40000),
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
	},]]
	thruster01 = ThrusterDef
	{
		force = 10000,
	},
	shield01 = ForceFieldDef
	{
		size = 50,
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