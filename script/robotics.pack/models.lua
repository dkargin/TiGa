-- models.lua
--
Models
{
	asteroid = fxAnimation2
	{
		file = "data/asteroid.png",
		frame = {64,64},
		fps = 30,
		mode = TiGa.Hold,
		realSize = 1.0,
	},
	tree = fxAnimation2{ file="data/units/plant01.png", realSize = {1.0, 1.0} , },
	vehicle = fxAnimation2{	file="data/units/4wheeled.png",	realSize = {0.5, 0.5} ,	},
	wbr914 = fxAnimation2
	{
		file = "data/units/wbt914.png",
		scale = 1,
		realSize = { 1.0, 1.0 },
		blend = binOR(TiGa.COLORMUL,TiGa.ALPHAADD,TiGa.NOZWRITE),
	},
	round = fxAnimation2
	{
		file="data/units/mr_round.png",
		realSize = 1,
		blend = binOR(TiGa.COLORMUL,TiGa.ALPHAADD,TiGa.NOZWRITE),
	},

	box = fxAnimationRC
	{
		file="data/_box.PNG",
		fps=20,
		frame={128,128},
		realSize = {1,1},
		mode=TiGa.Loop,
		blend = binOR(TiGa.COLORMUL,TiGa.ALPHABLEND, TiGa.NOZWRITE),		
	},

	pedestrian = fxSprite{ file = "data/pedestrian.png", pose = Pose(0,0,0) },
}
