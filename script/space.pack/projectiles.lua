-- units.lua
--

Projectiles
{
	cannon = ProjectileDef
	{
		fxIdle = Models.projectile01,
		--impact = Models.explosion01,
		damage = 20,
		livingTime = 3.0,
		velocity = 400,
	},
	missile = ProjectileDef
	{
		fxIdle = Models.projectile01,
		perception = PerceptionDef
		{
			fov=360,
			distance=400,
		},
		--impact = Models.explosion01,
		damage = 10,
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
		impact = Models.explosion01,
		damage = 20,
		livingTime = 3.0,
		velocity = 400,
	}
}