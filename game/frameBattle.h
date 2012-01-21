#ifndef _GAMEPLAY_WINDOW_H_
#define _GAMEPLAY_WINDOW_H_

#include "multiblock.h"

class Ship : public Multiblock
{
};

class World;

class GameplayWindow : public GUI::Object
{
public:	
	World * world;
	Game * game;
	GameplayWindow(Game * core);
	~GameplayWindow();

	void onUpdate( float dt );
	void onRender();
	void closeWorld();
	size_t createShip(ShipBlueprint * blueprint);
};

#endif