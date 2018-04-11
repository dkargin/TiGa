#include "game.h"
#include "frameBattle.h"
#include "world.h"
#include "multiblock.h"


Multiblock * Multiblock::createFractureBody()
{
	return NULL;
}

void GenerateShipGraphics(Fx::EntityPtr effect, ShipBlueprint * blueprint, Game * game)
{
	Fx::Rect bounds = blueprint->getBounds(*game->gameData);
	float centerX = (bounds.x1 + bounds.x2) * 0.5;
	float centerY = (bounds.y1 + bounds.y2) * 0.5;

	for( int i = 0; i < blueprint->blocks.size(); i++)
	{
		ShipBlueprint::Block & block = blueprint->blocks[i];
		TileSectionDesc & desc = game->gameData->sections[block.tileType];
		for( int y = block.y; y < block.y + desc.sizeY; y++)
			for( int x = block.x; x < block.x + desc.sizeX; x++)
			{
				Fx::EntityPtr ptr = game->gameData->getSectionSprite(desc.spriteId)->clone();
				ptr->setPose(Pose((x-centerX) * blueprint->tileSize, (y - centerY) * blueprint->tileSize, 0));
				effect->attach(ptr);
			}
	}
}

void AddShipToWorld(World * world, ShipBlueprint * blueprint, Game * game)
{
	sim::ObjectManager* manager = world->gameObjects;
	float tileSize = 50;
	Multiblock* multiblock = new Multiblock(manager);
	if(!blueprint->blocks.empty())
	{
		multiblock->init(Pose::zero(), world->dynamics);
	}
	//multiblock->effects = game->fxManager->fxHolder();
	multiblock->cellWidth = tileSize;
	for( int i = 0; i < blueprint->blocks.size(); i++)
	{
		ShipBlueprint::Block & block = blueprint->blocks[i];		
		TileSectionDesc & desc = game->gameData->sections[block.tileType];
		CellDesc cellData;
		cellData.health = 100;
		for( int y = block.y; y < block.y + desc.sizeY; y++)
			for( int x = block.x; x < block.x + desc.sizeX; x++)
				multiblock->createCell(x, y, cellData);
	}

	GenerateShipGraphics(multiblock->getGraphics(), blueprint, game);
}

GameplayWindow::GameplayWindow(Game * game)
	:GUI::Object(Fx::Rect(0,0,0,0)),game(game)
{
	setAlign(GUI::AlignExpand, GUI::AlignExpand);
	world = new World("another world",game);

	world->initSimulation(true);
#ifdef FIX_THIS
	world->initRenderer(game->getHGE());
#endif

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
