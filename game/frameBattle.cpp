#include "stdafx.h"
#include "game.h"
#include "frameBattle.h"
#include "world.h"
#include "multiblock.h"


Multiblock * Multiblock::createFractureBody()
{
	return NULL;
}

void AddShipToWorld(World * world, ShipBlueprint * blueprint, Game * game)
{
	ObjectManager * manager = world->gameObjects;
	float tileSize = 50;
	Multiblock * multiblock = new Multiblock(manager);
	if( blueprint->blocksCount > 0 )
	{
		multiblock->init(Pose2::zero(), world->dynamics);
	}
	multiblock->effects = game->fxManager->fxHolder();
	multiblock->cellWidth = tileSize;
	for( int i = 0; i < blueprint->blocksCount; i++)
	{
		ShipBlueprint::Block & block = blueprint->blocks[i];		
		TileSectionDesc & desc = game->gameData->sections[block.tileType];
		CellDesc cellData;
		cellData.health = 100;
		for( int y = block.y; y < block.y + desc.sizeY; y++)
			for( int x = block.x; x < block.x + desc.sizeX; x++)
			{
				multiblock->createCell(x, y, cellData);
				FxEffect::Pointer ptr = game->gameData->getSectionSprite(desc.spriteId);
				FxEffect::Pointer newPtr = ptr->clone();
				newPtr->setPose(Pose(x * tileSize, y * tileSize, 0));
				multiblock->effects->attach(newPtr);
			}
	}
}

GameplayWindow::GameplayWindow(Game * game)
	:GUI::Object(hgeRect(0,0,0,0)),game(game)
{
	setAlign(GUI::AlignExpand, GUI::AlignExpand);
	world = new World("another world",game);

	world->initSimulation(true);
	world->initRenderer(game->getHGE());

	 AddShipToWorld(world, game->selectedBlueprint, game);
}

GameplayWindow::~GameplayWindow()
{
	closeWorld();	
}

void GameplayWindow::closeWorld()
{
	if( world )
	{
		delete world;
		world = NULL;
	}
}

void GameplayWindow::onRender()
{
	if( world )
	{
		world->render();
	}
}

void GameplayWindow::onUpdate( float dt )
{
	if( world )
	{
		world->update(dt);
	}
}