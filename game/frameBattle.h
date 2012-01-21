#ifndef _GAMEPLAY_WINDOW_H_
#define _GAMEPLAY_WINDOW_H_

#include "multiblock.h"

class Ship : public Multiblock
{
};

class GameplayWindow : public GUI::Object
{
public:	
	Game * game;
	GameplayWindow(Game * core);
	size_t createShip(ShipBlueprint * blueprint);
};

#endif