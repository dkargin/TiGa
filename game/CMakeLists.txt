add_executable(game 
	Core.cpp 
	draw.cpp 
	frameBattle.cpp 
	frameHangar.cpp 
	frameMainMenu.cpp
	frameShipyard.cpp
	game.cpp
	hgeMain.cpp
	level.cpp
	luaTools.cpp
	multiblock.cpp
	navGraph.cpp
	shields.cpp
	world.cpp
)

target_link_libraries(game fx sim)