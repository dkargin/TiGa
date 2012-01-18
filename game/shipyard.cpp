#include "stdafx.h"
#include "game.h"
#include "shipyard.h"

ShipyardArea::ShipyardArea(ShipyardWindow * shipyard) : Object(hgeRect(0,0,0,0)), shipyard(shipyard)
{
	tileSize = 50;
	tileOffsetX = 0;
	tileOffsetY = 0;
}

ShipyardArea::~ShipyardArea()
{
}

size_t ShipyardArea::findTile( int x, int y )
{
	for( size_t i = 0; i < tiles.size(); ++i)
	{
		Tile & tile = tiles[i];
		if( tile.x == x && tile.y == y )
			return i;
	}
	return -1;
}

void ShipyardArea::addTile( int x, int y, FxSpritePtr sprite )
{
	if( findTile(x,y) != -1 )
		return;
	Tile tile;
	tile.x = x;
	tile.y = y;
	tile.sprite = sprite;
	tiles.push_back(tile);
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
	if( key == 0 && shipyard->controlMode == ShipyardWindow::CreateTile )
	{
		vec2i gridCoords = screenToLocal(vec);
		addTile(gridCoords[0], gridCoords[1], shipyard->sections[shipyard->pickedSection].sprite );
		shipyard->setControlMode(ShipyardWindow::Normal);
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

void ShipyardArea::onRender()
{
	hgeRect rect = getClientRect();

	int cellsX = (rect.x2 - rect.x1) / tileSize;
	int cellsY = (rect.y2 - rect.y1) / tileSize;

	for( size_t i = 0; i < tiles.size(); ++i)
	{
		Tile & tile = tiles[i];
		vec2i cellPos = screenCellCenter( vec2i(tile.x , tile.y ) );
		
		Pose2z pose( rect.x1 + (tile.x + tileOffsetX + 0.5) * tileSize,  rect.y1 + (tile.y + tileOffsetY + 0.5) * tileSize, 0, 0 );
		tile.sprite->render( pose );
	}

	hgeRect cellRect;

	vec2i selected = screenCellCenter( selectedCell );
	cellRect.x1 = rect.x1 + (selectedCell[0] + tileOffsetX ) * tileSize;
	cellRect.y1 = rect.y1 + (selectedCell[1] + tileOffsetY ) * tileSize;	
	cellRect.x2 = cellRect.x1 + tileSize;
	cellRect.y2 = cellRect.y1 + tileSize;

	drawRectSolid(hge, cellRect, color );

	// draw grid
	for( int x = rect.x1 + tileOffsetX * tileSize; x < rect.x2; x+= tileSize )
		hge->Gfx_RenderLine(x, rect.y1, x, rect.y2, color, 0 );

	for( int y = rect.y1 + tileOffsetY * tileSize; y < rect.y2; y+= tileSize )
		hge->Gfx_RenderLine(rect.x1, y, rect.x2, y, color, 0 );
}
/////////////////////////////////////////////////////////////////////////////////////
void addDoubleBlock(Game * game, std::vector<SectionDesc> & sections, const char * path )
{
	SectionDesc desc;
	desc.sizeX = 1;
	desc.sizeY = 1;
	desc.sprite = game->fxManager->fxSprite(path,0,0,0,0);	
	sections.push_back(desc);
	desc.sprite = desc.sprite->copy();
	desc.sprite->flipVer();
	sections.push_back(desc);
}
void initBlocks(Game * game, std::vector<SectionDesc> & sections)
{
	SectionDesc desc;
	desc.sizeX = 1;
	desc.sizeY = 1;	
	desc.sprite = game->fxManager->fxSprite("./data/blocks/AD_sec01.png",0,0,80,80);	sections.push_back(desc);
	desc.sprite = game->fxManager->fxSprite("./data/blocks/Syn_sec02.png",0,0,80,80);	sections.push_back(desc);
	
	addDoubleBlock(game, sections, "./data/blocks/Boba_sec01.png");
	addDoubleBlock(game, sections, "./data/blocks/Boba_sec02.png");
	addDoubleBlock(game, sections, "./data/blocks/Boba_sec03.png");
	addDoubleBlock(game, sections, "./data/blocks/Boba_sec04.png");
	addDoubleBlock(game, sections, "./data/blocks/Bluh_sec01.png");
	addDoubleBlock(game, sections, "./data/blocks/Bluh_sec02.png"); 
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec01.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec02.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec03.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec04.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec05.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec06.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec07.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec08.png" );
	addDoubleBlock(game, sections, "./data/blocks/Dan_sec09.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec01.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec02.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec03.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec04.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec05.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec06.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec07.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec08.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec09.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec10.png" );
	addDoubleBlock(game, sections, "./data/blocks/Syn_sec11.png" );
}

ShipyardWindow::ShipyardWindow(Game * game)
	:GUI::Object(hgeRect(0,0,0,0)), game(game), shipyardArea( this )
{
	color = ARGB(255,0,0,0);
	controlMode = Normal;
	
	initBlocks(game, sections);

	for( int i = 0; i < 10; i++)
	{
		SectionDesc desc;
		desc.sizeX = 1;
		desc.sizeY = 1;
		desc.sprite = game->fxManager->fxSprite("./data/pedestrian.png",0,0,64,64);
		sections.push_back(desc);
	}

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

void ShipyardWindow::setControlMode( ShipyardWindow::Mode mode )
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
	for( int i = 0; i < sections.size(); i++ )
	{
		SharedPtr<GUI::Button> button = new GUI::Button;
		button->sprite = sections[i].sprite;
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
		core->uiSetCursorEffect(0, sections[tileId].sprite.get());
		setControlMode( CreateTile );		
	}
	else
		controlMode = Normal;
}

void ShipyardWindow::showObjects()
{
	clearContents();	
}

/////////////////////////////////////////////////////////////////////////////////////
// Grid view control