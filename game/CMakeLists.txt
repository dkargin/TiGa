add_executable(game
	frameBattle.cpp
	frameHangar.cpp
	frameMainMenu.cpp
	frameShipyard.cpp
	game.cpp
	level.cpp
	multiblock.cpp
	navGraph.cpp
	shields.cpp
	world.cpp
)

target_link_libraries(game simengine)
