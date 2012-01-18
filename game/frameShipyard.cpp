#include "stdafx.h"
#include "game.h"
#include "frameShipyard.h"

void addDoubleBlock(Game * game, GameData & gameData, const char * path )
{
	FxSpritePtr sprite = game->fxManager->fxSprite(path,0,0,0,0);
	if( sprite == NULL )
		return;
	
	gameData.sprites.push_back(sprite);
	sprite = sprite->copy();
	sprite->flipVer();
	gameData.sprites.push_back(sprite);

	TileSectionDesc desc;
	desc.sizeX = 1;
	desc.sizeY = 1;		
	desc.spriteId = gameData.sprites.size() - 2;
	gameData.sections.push_back(desc);

	desc.sizeX = 1;
	desc.sizeY = 1;		
	desc.spriteId = gameData.sprites.size() - 1;
	gameData.sections.push_back(desc);
}

void initBlocks(Game * game, GameData & gameData)
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
	initBlocks(game, *this);
}

GameData::~GameData()
{
}

FxSpritePtr GameData::getSprite( size_t spriteId )
{
	if( spriteId >= sprites.size() )
	{
		return NULL;
	}
	return sprites[spriteId];
}

FxSpritePtr GameData::getSectionSprite(size_t sectionType)
{
	if( sectionType >= sections.size() )
	{
		return NULL;
	}
	return getSprite(sections[sectionType].spriteId);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ShipyardArea::ShipyardArea(ShipyardWindow * shipyard) : Object(hgeRect(0,0,0,0)), shipyard(shipyard)
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
	for( size_t i = 0; i < blueprint.blocksCount; ++i )
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
	hgeRect rect = getClientRect();
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
	hgeRect rect = getClientRect();
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
	TileSectionDesc & desc = shipyard->game->gameData->sections[tileId];
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

void ShipyardArea::onRender()
{
	hgeRect rect = getClientRect();

	int cellsX = (rect.x2 - rect.x1) / tileSize;
	int cellsY = (rect.y2 - rect.y1) / tileSize;

	hgeRect cellRect;

	vec2i selected = screenCellCenter( selectedCell );
	cellRect.x1 = rect.x1 + (selectedCell[0] + tileOffsetX ) * tileSize;
	cellRect.y1 = rect.y1 + (selectedCell[1] + tileOffsetY ) * tileSize;	
	cellRect.x2 = cellRect.x1 + tileSize;
	cellRect.y2 = cellRect.y1 + tileSize;

	// draw bounding rect
	drawRectSolid(hge, cellRect, color );

	// draw grid
	for( int x = rect.x1 + tileOffsetX * tileSize; x < rect.x2; x+= tileSize )
		hge->Gfx_RenderLine(x, rect.y1, x, rect.y2, color );

	for( int y = rect.y1 + tileOffsetY * tileSize; y < rect.y2; y+= tileSize )
		hge->Gfx_RenderLine(rect.x1, y, rect.x2, y, color );
	// draw blocks
	for( size_t i = 0; i < blueprint.blocksCount; ++i)
	{
		ShipBlueprint::Block & block = blueprint.blocks[i];
		const TileSectionDesc & desc = getSectionDesc(block.tileType);
		vec2i cellPos = screenCellCenter( vec2i(block.x , block.y ) );		
		Pose2z pose( rect.x1 + (block.x + tileOffsetX + (float)desc.sizeX * 0.5) * tileSize,  rect.y1 + (block.y + tileOffsetY + (float)desc.sizeX * 0.5) * tileSize, 0, 0 );
		shipyard->getSectionSprite(block.tileType)->render(pose);
	}	
}
/////////////////////////////////////////////////////////////////////////////////////
ShipyardWindow::ShipyardWindow(Game * game)
	:GUI::Object(hgeRect(0,0,0,0)), game(game), shipyardArea( this )
{
	color = ARGB(255,0,0,0);
	controlMode = Normal;
	

	menu->setDesiredSize(buttonWidth, buttonHeight);
	menu->color = clrButtons;
	menu->onPressed = [=]()
	{
		game->showMainMenu();
	};

	menu->setText("Exit", game->font);
	insert(menu, GUI::AlignMax, GUI::AlignMin);

	const float sliderWidth = 10;
	const float toolboxWidth = buttonWidth * 2 + sliderWidth;
	
	toolbox->setDesiredSize(toolboxWidth, 0);

	toolbox->drawFrame = true;

	toolbox->color = ARGB(255,0,0,0);
	toolbox->clrFrame = ARGB(255,0,255,0);
	toolbox->slideVer = true;

	font = game->font;

	insert(toolbox, GUI::AlignMin, GUI::AlignExpand);

	tiles->setText("Tiles", game->font);
	tiles->setDesiredSize(buttonWidth, buttonHeight);
	tiles->onPressed = [=](){showTiles();};
	toolbox->insert(tiles, GUI::AlignMin, GUI::AlignMin);

	objects->setText("Objects", game->font);
	objects->setDesiredSize(buttonWidth, buttonHeight);
	objects->onPressed = [=](){showObjects();};
	toolbox->insert(objects, GUI::AlignMax, GUI::AlignMin);
	

	toolboxSlider->sprite = game->fxManager->fxSolidQuad(10, 50, ARGB(255,255,0,0));
	toolboxSlider->setMode(0, 100, 0);
	toolboxSlider->setDesiredPos( 0, 0 );
	toolboxSlider->setDesiredSize( sliderWidth, 0 );
	toolboxSlider->onStateChange = [=]( GUI::Slider * slider, float value, float min, float max )
	{
		toolbox->setOffsetVer( value );
	};
	toolbox->insert(toolboxSlider, GUI::AlignMin, GUI::AlignExpand);

	shipyardArea->setDesiredPos( toolboxWidth, 0 );
	shipyardArea->setDesiredSize( core->getScreenWidth() - toolboxWidth, core->getScreenHeight() );
	
	insert(shipyardArea, GUI::AlignMax, GUI::AlignExpand);
}

void ShipyardWindow::clearContents()
{	
	contents.clear();
}

void ShipyardWindow::setControlMode( ShipyardEditMode mode )
{
	controlMode = mode;		
	if( controlMode == Normal )
	{
		core->uiResetCursorEffect(0);
	}	
}

void ShipyardWindow::addListboxItem( GUI::Object * object )
{
	hgeRect containerRect = toolbox->getClientRect();
	hgeRect rect = object->getRect();

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
		SharedPtr<GUI::Button> button = new GUI::Button;
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
		core->uiSetCursorEffect(0, getSectionSprite(tileId));
		setControlMode( CreateTile );		
	}
	else
		controlMode = Normal;
}

void ShipyardWindow::showObjects()
{
	clearContents();	
}

FxSpritePtr ShipyardWindow::getSectionSprite(size_t sectionType)
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