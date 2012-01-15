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
		muzzleOffset = 35,
		timeShootDelay = 0.3,
		maxAmmo = 5,
		timeReload = 5,
		spread = 0,
		--fxShoot=fxAnimation{name="Turret01",scale=0.5},
		fxShoot = Models.smallTurret,		
	},
	shotgun = WeaponTurretDef
	{
		projectile = Projectiles.cannon,
		weaponType = TiGa.WeaponDef_weaponProjectile,
		velocity = 600,
		dimensions = 180,
		muzzleOffset = 35,
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
		muzzleOffset = 35,
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
		muzzleOffset=5,
		timeShootDelay=0.3,
		timeShootDuration=0.2,
		maxAmmo=5,
		timeReload=5,
		spread=2,
		fxShoot = Models.smallTurret,
		external = true,
	},
	mover01 = MoverVehicleDef
	{
		maxVelocity = TiGa.vec2f(2,5),
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
		maxVelocity = TiGa.vec2f(2,5),
		acceleration = TiGa.vec2f(2,5),
		kinematic = true,
		moveBack = true,
		--brakeForce={1,1}
	},
	mover03 = MoverVehicleDef
	{
		maxVelocity = TiGa.vec2f(2,5),
		acceleration = TiGa.vec2f(2,5),
		kinematic = true,
		moveBack = true,
		--brakeForce={1,1}
	},
	mover_ch01 = MoverCharacterDef
	{
		kinematic = true,
		speedLinear = 2,
		speedAngular = 5,
	},
	mover_ch02 = MoverCharacterDef
	{
		kinematic = false,
		speedLinear = 2,
		speedAngular = 5,
		thrust = 100,
		torque = 40,
	},
	thruster01 = ThrusterDef
	{
		force = 10,
	},
	shield01 = ForceFieldDef
	{
		size = 2,
	},
	perception360 = PerceptionDef
	{
		fov = 360,
		distance=5,
	},
	perception_far = PerceptionDef
	{
		fov = 360,
		distance = 17,
	},
	perception240 = PerceptionDef
	{
		fov=240,
		distance=6,
	},
	targeting01 = TargetingSystemDef{},
}