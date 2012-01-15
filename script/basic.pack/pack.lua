-- pack.lua
-- specify some common sprites used in level editor
Models
{
	cursor_default = fxSprite{ file = "data/cursor.png", },
	cursor_green = fxSprite{ file = "data/walk.png", },
	cursor_red = fxSprite{ file = "data/target.png", },
	--[[cursor_selection = fxAnimationRC
	{
		file = "data/Effects/Icons_Selection.png",
		rect = {0,128,128,128},
		frame = {128,128},
		fps = 20,
		blend = binOR(TiGa.COLORADD,TiGa.ALPHAADD, TiGa.NOZWRITE),
		mode = Once,
		scale = 0.25,
	},
	selection = fxAnimation2
	{
		file = "data/selection.PNG",
		frame = {32,32},
		realSize = {1,1},
		fps = 10,
		pose =Pose{0,0,0,10},
	},]]
}
