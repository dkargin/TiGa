-- units.lua
--

Projectiles
{
	cannon = ProjectileDef
	{
		fxIdle = Models.projectile01,
		--impact = Models.explosion01,
		damage = 0,
		livingTime = 3.0,
		velocity = 4,
	},
	missile = ProjectileDef
	{
		fxIdle = Models.projectile01,
		perception = PerceptionDef
		{
			fov=360,
			distance = 4,
		},
		--impact = Models.explosion01,
		damage = 0,
		livingTime = 3.0,
		velocity = 200,
	},
	energy = ProjectileDef
	{
		fxIdle = Models.pulse01,
		impact = Models.explosion01,

		--impact = fxArray
		--{
		--	Models.explosion01,
		--	Models.flare01,
		--	Models.explosion_smoke,
		--},

		livingTime = 12.0,
		velocity = 100,
	},
	cannon_trail = ProjectileDef
	{
		fxIdle = Models.projectile01,
		--fxIdle = fxArray
		--{
		--	Models.projectile01,
		--	--Models.particlesTest,
		--},
		impact = Models.explosion02,
		damage = 0,
		livingTime = 3.0,
		velocity = 20,
	}
}