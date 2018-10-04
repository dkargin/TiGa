#include "game.h"
#include <simengine/fx/fxobjects.h>
#include "frameShipyard.h"

void addDoubleBlock(Game * game, GameData * gameData, const char * path )
{
	Fx::FxSpritePtr sprite = game->fxManager->fxSprite(path,0,0,0,0);
	if (!sprite)
		return;
	
	gameData->sprites.push_back(sprite->clone());
	sprite->flipVer();
	gameData->sprites.push_back(sprite);

	TileSectionDesc desc;
	desc.sizeX = 1;
	desc.sizeY = 1;		
	desc.spriteId = gameData->sprites.size() - 2;
	gameData->sections.push_back(desc);

	desc.sizeX = 1;
	desc.sizeY = 1;		
	desc.spriteId = gameData->sprites.size() - 1;
	gameData->sections.push_back(desc);
}

void initBlocks(Game * game, GameData * gameData)
{
	//TileSectionDesc desc;
	//desc.sizeX = 1;
	//desc.sizeY = 1;	
	//desc.sprite = game->fxManager->fxSprite("./data/blocks/AD_sec01.png",0,0,80,80);	sections.push_back(desc);
	//desc.sprite = game->fxManager->fxSprite("./data/blocks/Syn_sec02.png",0,0,80,80);	sections.push_back(desc);
	
	addDoubleBlock(game, gameData, "./data/blocks/Boba_sec01.png");
	addDoubleBlock(game, gameData, "./data/blocks/Boba_sec02.png");
	addDoubleBlock(game, gameData, "./data/blocks/Boba_sec03.png");
	addDoubleBlock(game, gameData, "./data/blocks/Boba_sec04.png");
	addDoubleBlock(game, gameData, "./data/blocks/Bluh_sec01.png");
	addDoubleBlock(game, gameData, "./data/blocks/Bluh_sec02.png"); 
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec01.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec02.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec03.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec04.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec05.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec06.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec07.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec08.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Dan_sec09.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec01.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec02.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec03.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec04.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec05.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec06.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec07.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec08.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec09.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec10.png" );
	addDoubleBlock(game, gameData, "./data/blocks/Syn_sec11.png" );
}
//////////////////////////////////////////////////////////////////////////////////////////////
GameData::GameData(Game * game)
{
	initBlocks(game, this);

	blueprintsCount = 4;
	blueprints = new ShipBlueprint[blueprintsCount];
}

GameData::~GameData()
{
	if( blueprints )
		delete []blueprints;
}

Fx::EntityPtr GameData::getSprite( size_t spriteId )
{
	if( spriteId >= sprites.size() )
	{
		return NULL;
	}
	return sprites[spriteId];
}

Fx::EntityPtr GameData::getSectionSprite(size_t sectionType)
{
	if( sectionType >= sections.size() )
	{
		return NULL;
	}
	return getSprite(sections[sectionType].spriteId);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ShipyardArea::ShipyardArea(ShipyardWindow * shipyard):
	Object(Fx::Rect(0,0,0,0)),
	shipyard(shipyard)
{
	tileSize = 50;
	tileOffsetX = 0;
	tileOffsetY = 0;

	cellsWidth = ShipyardCellsMax;
	cellsHeight = ShipyardCellsMax;
	cellsCenterX = 0;//cellsWidth / 2;
	cellsCenterY = 0;//cellsHeight / 2;
	updateCells();
}

ShipyardArea::~ShipyardArea()
{
}


const TileSectionDesc & ShipyardArea::getSectionDesc(size_t id) const
{
	return shipyard->game->gameData->sections[id];
}

void ShipyardArea::updateCells()
{
	// clear cells map
	for( size_t i = 0; i < cellsWidth * cellsHeight; ++i )
	{
		cells[i].objectId = -1;
		cells[i].blockId = -1;
	}
	// fill by tile id
	for( size_t i = 0; i < blueprint.blocks.size(); ++i )
	{
		const ShipBlueprint::Block & block = blueprint.blocks[i];
		const TileSectionDesc & desc = getSectionDesc(block.tileType);
		for( int y = block.y + cellsCenterY; y < block.y + cellsCenterY + desc.sizeY; ++y )
		{
			for( int  x = block.x + cellsCenterX; x < block.x + cellsCenterX + desc.sizeX; ++x)
			{
				cells[x + y * cellsWidth].blockId = i;
			}
		}
	}
}

void ShipyardArea::addTile( int x, int y, size_t pickedTile )
{
	if(!canPlaceTile(x, y, pickedTile))
		return;	
	ShipBlueprint::Block block;
	block.direction = 0;
	block.tileType = pickedTile;
	block.x = x;
	block.y = y;
	blueprint.addBlock(&block);
	updateCells();
}


vec2i ShipyardArea::screenToLocal( const uiVec & vec )
{
	Fx::Rect rect = getClientRect();
	// cells on screen
	float x = (vec[0] - rect.x1) / tileSize - tileOffsetX;
	float y = (vec[1] - rect.y1) / tileSize - tileOffsetY;
	return vec2i( x, y );
}

bool ShipyardArea::onMouse(int mouseId, int key, int state, const uiVec & vec)
{
	selectedCell = screenToLocal(vec);	
	if( key == 0 && shipyard->controlMode == ShipyardEditMode::CreateTile )
	{
		vec2i gridCoords = screenToLocal(vec);
		bool valid = canPlaceTile( gridCoords[0], gridCoords[1], shipyard->pickedSection);		
		addTile(gridCoords[0], gridCoords[1], shipyard->pickedSection);
		shipyard->setControlMode(ShipyardEditMode::Normal);
	}
	return true;
}

bool ShipyardArea::onMouseMove( int mouseId, const uiVec & vec, ShipyardArea::MoveState state)
{
	selectedCell = screenToLocal(vec);
	return true;
}


vec2i ShipyardArea::screenCellCenter( const vec2i & cell ) const
{
	Fx::Rect rect = getClientRect();
	// cells on screen
	int cellsX = (rect.x2 - rect.x1) / tileSize;
	int cellsY = (rect.y2 - rect.y1) / tileSize;
	// coordinates of left top cell corner
	int cornerX = (rect.x2 + rect.x1 - cellsX * tileSize ) / 2;
	int cornerY = (rect.y2 + rect.y1 - cellsY * tileSize ) / 2;

	return vec2i(cornerX + ( cell[0] + cellsX * 0.5 - 0.5) * tileSize, cornerY + (cell[1] + cellsY * 0.5 - 0.5) * tileSize);
}

void ShipyardArea::setBlueprint( ShipBlueprint * blueprint, bool locked )
{
	this->blueprint.copy(blueprint);
	updateCells();
}

void ShipyardArea::getBlueprint( ShipBlueprint * blueprint )
{
	blueprint->copy(&this->blueprint);
}

size_t ShipyardArea::findTile(int cx, int cy)
{
	if( cy < cellsCenterY || cx < cellsCenterX || cx - cellsCenterX >= cellsWidth || cy - cellsCenterY >= cellsHeight )
		return -1;
	return cells[cx + cellsCenterX + (cy + cellsCenterY) * cellsWidth].blockId;
}


bool ShipyardArea::canPlaceTile(int cx, int cy, size_t tileId)
{
	TileSectionDesc& desc = shipyard->game->gameData->sections[tileId];
	// bounds checking
	if( cy + cellsCenterY < 0 || cx + cellsCenterX < 0 || cx + cellsCenterX >= cellsWidth || cy + cellsCenterY >= cellsHeight )
		return false;
	for( int y = cy + cellsCenterY; y < cy + cellsCenterY + desc.sizeY; ++y )
	{
		for( int x = cx + cellsCenterX; x < cx + cellsCenterX + desc.sizeX; ++x)
		{
			if(cells[x + y * cellsWidth].blockId != -1)
				return false;
		}
	}
	// check if there are any neighbours
	return true;
}

void DrawBlueprint(Fx::RenderContext* rc, ShipBlueprint * blueprint, GameData * data, float cornerX, float cornerY, float tileSize, float scale )
{
	// draw blocks
	for( size_t i = 0; i < blueprint->blocks.size(); ++i)
	{
		ShipBlueprint::Block & block = blueprint->blocks[i];
		const TileSectionDesc & desc = data->sections[block.tileType];	
		Fx::Pose pose(
				cornerX + (block.x + (float)desc.sizeX * 0.5) * tileSize,
				cornerY + (block.y + (float)desc.sizeX * 0.5) * tileSize, 0, 0 );
		Fx::EntityPtr ptr = data->getSectionSprite(block.tileType);
		ptr->render(rc, pose);
	}
}

void ShipyardArea::onRender(Fx::RenderContext* rc)
{
	Fx::Rect rect = getClientRect();

	int cellsX = (rect.x2 - rect.x1) / tileSize;
	int cellsY = (rect.y2 - rect.y1) / tileSize;

	Fx::Rect cellRect;

	vec2i selected = screenCellCenter( selectedCell );
	cellRect.x1 = rect.x1 + (selectedCell[0] + tileOffsetX ) * tileSize;
	cellRect.y1 = rect.y1 + (selectedCell[1] + tileOffsetY ) * tileSize;	
	cellRect.x2 = cellRect.x1 + tileSize;
	cellRect.y2 = cellRect.y1 + tileSize;

#ifdef FIX_THIS
	// draw bounding rect
	drawRectSolid(rc, cellRect, color );

	// draw grid
	for( int x = rect.x1 + tileOffsetX * tileSize; x < rect.x2; x+= tileSize )
		hge->Gfx_RenderLine(x, rect.y1, x, rect.y2, color );

	for( int y = rect.y1 + tileOffsetY * tileSize; y < rect.y2; y+= tileSize )
		hge->Gfx_RenderLine(rect.x1, y, rect.x2, y, color );
#endif
	// draw blueprint
	DrawBlueprint(rc, &blueprint, shipyard->game->gameData.get(), rect.x1, rect.y1, tileSize, 1.0);
}

/////////////////////////////////////////////////////////////////////////////////////
ShipyardWindow::ShipyardWindow(Game * game)
	:GUI::Object(Fx::Rect(0,0,0,0)), game(game)
{
	shipyardArea = std::make_shared<ShipyardArea>(this);
	setAlign(GUI::AlignExpand, GUI::AlignExpand);
	color = Fx::makeARGB(255,0,0,0);
	controlMode = Normal;

	menu->setDesiredSize(buttonWidth, buttonHeight);
	menu->color = clrButtons;
	menu->onPressed = [=]()
	{
		frameBack();
	};
	menu->setText("Exit", game->font);
	menu->setAlign(GUI::AlignMax, GUI::AlignMin);
	insert(menu);

	const float sliderWidth = 10;
	const float toolboxWidth = buttonWidth * 2 + sliderWidth;
	
	toolbox->setDesiredSize(toolboxWidth, 0);

	toolbox->drawFrame = true;

	toolbox->color = Fx::makeARGB(255,0,0,0);
	toolbox->clrFrame = Fx::makeARGB(255,0,255,0);
	toolbox->slideVer = true;

	font = game->font;
	toolbox->setAlign(GUI::AlignMin, GUI::AlignExpand);
	insert(toolbox);

	tiles->setText("Tiles", game->font);
	tiles->setDesiredSize(buttonWidth, buttonHeight);
	tiles->onPressed = [=](){showTiles();};
	tiles->setAlign(GUI::AlignMin, GUI::AlignMin);
	toolbox->insert(tiles);

	objects->setText("Objects", game->font);
	objects->setDesiredSize(buttonWidth, buttonHeight);
	objects->onPressed = [=](){showObjects();};
	objects->setAlign(GUI::AlignMax, GUI::AlignMin);	
	toolbox->insert(objects);

	toolboxSlider->sprite = game->fxManager->fxSolidQuad(10, 50, Fx::makeARGB(255,255,0,0));
	toolboxSlider->setMode(0, 100, 0);
	toolboxSlider->setDesiredPos( 0, 0 );
	toolboxSlider->setDesiredSize( sliderWidth, 0 );
	toolboxSlider->onStateChange = [=]( GUI::Slider * slider, float value, float min, float max )
	{
		toolbox->setOffsetVer( value );
	};
	toolbox->setAlign(GUI::AlignMin, GUI::AlignExpand);
	toolbox->insert(toolboxSlider);

	shipyardArea->setDesiredPos( toolboxWidth, 0 );
	shipyardArea->setDesiredSize(
			game->getScreenWidth() - toolboxWidth,
			game->getScreenHeight());
	shipyardArea->setAlign(GUI::AlignMax, GUI::AlignExpand);
	insert(shipyardArea);

	
}

void ShipyardWindow::clearContents()
{	
	contents.clear();
}

void ShipyardWindow::frameBack()
{
	if( game->selectedBlueprint != NULL )
	{
		getBlueprint(game->selectedBlueprint);
		game->selectedBlueprint = NULL;
	}	
	game->showHangar();
}

void ShipyardWindow::setControlMode( ShipyardEditMode mode )
{
	controlMode = mode;		
	if( controlMode == Normal )
	{
		game->uiResetCursorEffect(0);
	}	
}

void ShipyardWindow::addListboxItem(GUI::ObjectPtr object )
{
	Fx::Rect containerRect = toolbox->getClientRect();
	Fx::Rect rect = object->getRect();

	const int cols = 2;
	int cellWidth = containerRect.width() / cols;
	const int cellHeight = 64;
	int size = contents.size();
	int cellX = size % cols, cellY = size / cols + 1;
	object->setDesiredPos( cellX * cellWidth, cellY * cellHeight );
	object->setDesiredSize( cellWidth, cellHeight );
	contents.push_back( object );
	toolbox->insert( object );
}

void ShipyardWindow::showTiles()
{
	clearContents();
	for( int i = 0; i < game->gameData->sections.size(); i++ )
	{
		GUI::ButtonPtr button(new GUI::Button);
		button->sprite = game->gameData->getSectionSprite(i);
		button->onPressed = [=]()
		{
			selectTile(i);
		};
		addListboxItem(button);
	}
}

void ShipyardWindow::selectTile( size_t tileId )
{
	if( controlMode == Normal )
	{
		pickedSection = tileId;
		game->uiSetCursorEffect(0, getSectionSprite(tileId));
		setControlMode( CreateTile );
	}
	else
		controlMode = Normal;
}

void ShipyardWindow::showObjects()
{
	clearContents();	
}

Fx::EntityPtr ShipyardWindow::getSectionSprite(size_t sectionType)
{
	if(game->gameData)
		return game->gameData->getSectionSprite(sectionType);
	else
		return NULL;
}

void ShipyardWindow::setBlueprint(ShipBlueprint * blueprint, bool locked)
{
	assert(blueprint != NULL);
	shipyardArea->setBlueprint(blueprint, locked);
}

void ShipyardWindow::getBlueprint(ShipBlueprint * blueprint)
{
	assert(blueprint != NULL);
	shipyardArea->getBlueprint(blueprint);
}
/////////////////////////////////////////////////////////////////////////////////////
// Grid view control
