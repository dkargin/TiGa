-- scenes.lua
-- register all test scenes
function createRandom(def,cx,cy,width,height,delta)
	local randScale = 100
	for i=1,width do
		for j=1,height do
			local x=cx+i*delta + randRange(- delta * randScale, delta * randScale) / (2 * randScale)
			local y=cy+j*delta + randRange(- delta * randScale, delta * randScale) / (2 * randScale)
			local dirX = randRange(-50,50)
			local dirY = randRange(-50,50)
			createUnit(def,{x,y,0},{dirX,dirY,0},PlayerID.neutral)
		end
	end
end

Scene
{
	name = "Shooting range",
	unitType = "tank",
	load = function(self)
		self.unit = self:createUnit(self.unitType,{-4,0,90},PlayerID.player)
		chaser = self:createUnit(self.unitType,{4,0,0},PlayerID.berserk)
		--chaser:getController():cmdAttack(self.unit)

		takeControl(self.unit)
		print("player is created")
		view:follow(self.unit,true,false)
		print("camera is attached")
		createRandom('asteroid',-20,-20,10,10, 4)
		print("asteroid field is created")
		createUnit('asteroid',{-100, 30,0},{1,0,0},PlayerID.neutral)
	end,
}

Scene
{
	name = "Battle demo",
	chaserType = "tank",
	preyType = "tank",
	load = function(self)
		chaser = self:createUnit(self.chaserType,{-4,0,0},2)
		prey = self:createUnit(self.preyType,{4,0,180},3)
		prey:getController():cmdGuard(4,0,1)
		chaser:getController():cmdAttack(prey)
		--createRandom('asteroid',-1000,-1000,10,10,200)
		--createUnit('asteroid',{-100, 30,0},{1,0,0},PlayerID.neutral)
	end,
}

Scene
{
	name = "Manual Character",
	load = function(self)
		takeControl(createUnit('spaceship02',{-200,-200,5},{0,1,0},PlayerID.player))
	end,
}

Scene
{
	name = "Mussile launch",
	load = function(self)
		takeControl(createUnit('rocket01',{-200,-200,5},{0,1,0},PlayerID.player))
		createUnit('tree',{-100, 30,0},{1,0,0},PlayerID.neutral)
	end,
}

Scene
{
	name = "Manual vehicle",
	load = function(self)
		--createRandom('asteroid',-1000,-1000,10,10,200)
		takeControl(self:createUnit('robot',{-100,-200,0},PlayerID.player))
		--createUnit('tree',{-250,0,0},{1,0,0},PlayerID.neutral)
		--createUnit('tree',{0,-100,0},{1,0,0},PlayerID.neutral)
		--createUnit('tree',{0, 100,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-100, 30,0},{1,0,0},PlayerID.neutral)
		--createUnit('tree',{-100,-30,0},{1,0,0},PlayerID.neutral)
		createUnit('box',{0, 0 ,0},{1,0,0},PlayerID.neutral)
	end,
}

Scene
{
	name = "Steering Cross",
	load = function(self)
		local tank=self:createUnit('robot',{-4,0,0},PlayerID.NPC,TiGa.createPerceptionAI)
		tank:getController():cmdPatrol( 4,0,0)

		local tank=self:createUnit('robot',{ 4,0,180},PlayerID.NPC,TiGa.createPerceptionAI)
		tank:getController():cmdPatrol(-4,0,0)

		local tank=self:createUnit('robot',{0,-4,0},PlayerID.NPC,TiGa.createPerceptionAI)
		tank:getController():cmdPatrol(0, 4,0)

		local tank=self:createUnit('robot',{0, 4, -90},PlayerID.NPC,TiGa.createPerceptionAI)
		tank:getController():cmdPatrol(0,-4,0)

		local tank=self:createUnit('robot',{ 2, 2,-90},PlayerID.NPC,TiGa.createPerceptionAI)
		tank:getController():cmdPatrol( 2,-2,0)
		tank:getController():cmdPatrol(-2,-2,0)
		tank:getController():cmdPatrol(-2, 2,0)

		createUnit('tree',{0,0,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-1,-1,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{ 1,-1,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{ 1, 1,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-1, 1,0},{1,0,0},PlayerID.neutral)

		function createPlayer(pnum)
			debugTrace('Creating player')
			takeControl(createUnit('tank',{0,-500,5},{0,1,0},pnum))
		end

		--createPlayer(PlayerID.player)
	end,
}

Scene
{
	name = "static steering test",
	load = function(self)

		local tank = self:createUnit('robot',{-8,0,0},PlayerID.NPC,TiGa.createPerceptionAI)

		tank:getController():cmdPatrol(4,0,0)
		tank:getController():cmdPatrol(-8,0,0)
		--[[
		tank:getController():cmdMoveTo(200,0,0)
		tank:getController():cmdMoveTo(-400,0,0)
		tank:getController():cmdMoveTo(100,-100,0)
		]]--
		createUnit('tree',{-5.0,0,0},{1,0,0},PlayerID.neutral)

		--[[createUnit('tree',{0,-100,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{0, 100,0},{1,0,0},PlayerID.neutral)]]--
		createUnit('tree',{-2.00, 2,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-2.00,-2,0},{1,0,0},PlayerID.neutral)

		function onMouseClick(key,x,y)
			tank:getController():cmdMoveTo(x,y,0)
		end
	end,
}

Scene
{
	name = "two robots steering",
	load = function(self)

		self:createUnit('robot',{-6, 0.1,  0},PlayerID.NPC,TiGa.createPerceptionAI):getController():cmdPatrol( 6, 0.1,0)
		self:createUnit('robot',{ 6,-0.1,180},PlayerID.NPC,TiGa.createPerceptionAI):getController():cmdPatrol(-6,-0.1,0)
		
		--function onMouseClick(key,x,y)
		--	tank:getController():cmdMoveTo(x,y,0)
		--end
	end,
}

Scene
{
	name = "two robots steering 2",
	load = function(self)

		self:createUnit('robot',{-6, 0,  0},PlayerID.NPC,TiGa.createPerceptionAI):getController():cmdPatrol( 6, 0,0)
		self:createUnit('robot',{ 6, 0,180},PlayerID.NPC,TiGa.createPerceptionAI):getController():cmdPatrol(-6, 0,0)
		
		--function onMouseClick(key,x,y)
		--	tank:getController():cmdMoveTo(x,y,0)
		--end
	end,
}

Scene
{
	name = "Building",
	load = function(self)
		world:initMap("data/hospital.png",0.25,false,TiGa.Pose(0,0,0,-1))
		local tank = self:createUnit('robot',{2,0,0},PlayerID.NPC,TiGa.createPerceptionAI)
		tank:getController():cmdMoveTo(3,0,0)
		--[[
		tank:getController():cmdMoveTo(200,0,0)
		tank:getController():cmdMoveTo(-400,0,0)
		tank:getController():cmdMoveTo(100,-100,0)

		createUnit('tree',{-250,0,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{0,-100,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{0, 100,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-100, 30,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-100,-30,0},{1,0,0},PlayerID.neutral)
		]]--
		function onMouseClick(key,x,y)
			tank:getController():cmdMoveTo(x,y,0)
		end
	end,
}

sceneSimple = Scene
{
	name = "Simple Building",
	load = function(self)
		world:initMap("data/simple_building.png",5,true,TiGa.Pose(0,0,0,-10))
		local tank = self:createUnit('robot',{2,0,0},PlayerID.NPC,TiGa.createPerceptionAI)
		--tank:getController():cmdMoveTo(300,0,0)
--[[
		local tank1 = self:createUnit('robot',{-130,160,0},PlayerID.NPC,TiGa.createPerceptionAI)
		tank1:getController():cmdPatrol(-435,-395,0)
		
		tank:getController():cmdMoveTo(200,0,0)
		tank:getController():cmdMoveTo(-400,0,0)
		tank:getController():cmdMoveTo(100,-100,0)

		createUnit('tree',{-250,0,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{0,-100,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{0, 100,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-100, 30,0},{1,0,0},PlayerID.neutral)
		createUnit('tree',{-100,-30,0},{1,0,0},PlayerID.neutral)
		]]--
		function onMouseClick(key,x,y)
			tank:getController():cmdMoveTo(x,y,0)
		end
	end,
}

Scene
{
	name = "steering in the wood",
	load = function(self)
		createRandom('tree',-400,-400,40,40,150)
		local tank = self:createUnit('robot',{-800,0,5},PlayerID.NPC,TiGa.createPerceptionAI)
		tank:getController():cmdMoveTo(800,000,0)
		tank:getController():cmdMoveTo(800,-800,0)
		tank:getController():cmdMoveTo(0,-800,0)
		--tank:getController():cmdMoveTo(400,0,0)
	end
}

Scene
{
	name = "circle steering 7",
	load = function(self)
		local count = 7
		local posX = 0
		local posY = 0
		local radius = 10
		for i = 1, count do
			local angle = i * 360 / count
			local cs = math.cos(math.rad(angle))
			local sn = math.sin(math.rad(angle))
			local tank = self:createUnit('robot',{posX + radius * cs, posY + radius * sn, angle},PlayerID.NPC,TiGa.createPerceptionAI)
			tank:getController():cmdMoveTo(posX - radius * cs, posY - radius * sn, 0)			
		end		
		--tank:getController():cmdMoveTo(400,0,0)
	end
}

Scene
{
	name = "circle steering 15",
	load = function(self)
		local count = 15
		local posX = 0
		local posY = 0
		local radius = 20
		for i = 1, count do
			local angle = i * 360 / count
			local cs = math.cos(math.rad(angle))
			local sn = math.sin(math.rad(angle))
			local tank = self:createUnit('robot',{posX + radius * cs, posY + radius * sn, angle + 180},PlayerID.NPC,TiGa.createPerceptionAI)
			tank:getController():cmdMoveTo(posX - radius * cs, posY - radius * sn, 0)			
		end		
		--tank:getController():cmdMoveTo(400,0,0)
	end
}
Scene
{
	name = "circle steering 32",
	load = function(self)
		view.scale = 25
		local count = 32
		local posX = 0
		local posY = 0
		local radius = 12
		for i = 1, count do
			local angle = i * 360 / count
			local cs = math.cos(math.rad(angle))
			local sn = math.sin(math.rad(angle))
			local tank = self:createUnit('robot',{posX + radius * cs, posY + radius * sn, angle + 180},PlayerID.NPC,TiGa.createPerceptionAI)
			tank:getController():cmdMoveTo(posX - radius * cs, posY - radius * sn, 0)			
		end		
		--tank:getController():cmdMoveTo(400,0,0)
	end
}
