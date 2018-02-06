#include "core.h"
#include "world.h"
#include "game.h"

#include "frameMainMenu.h"
#include "frameHangar.h"

int hgeMain()
{
	Game game;
	game.executeScript("./script/startup_Game.lua");
	game.font = GUI::FontPtr(new hgeFont("./data/font1.fnt"));
	game.initGame();
	game.run();	
	return 0;
}

/////////////////////////////////////////////////////////////////////
// Game class implementation
/////////////////////////////////////////////////////////////////////
Game::Game()
	:active(nullptr)
{
	selectedBlueprint = nullptr;
}

Game::~Game()
{
	gameData = NULL;
}
/*
class TestWindow : public GUI::Object
{
public:
	Instance<GUI::Frame> frame;
	SharedPtr<GUI::Button> button;
	TestWindow()
		:Object(hgeRect())
	{
		//frame = new GUI::Frame;
		insert(frame);
		button = new GUI::Button;
		frame->insert(button);
	}
	~TestWindow()
	{
	}
};
*/
void Game::initGame()
{
	gameData = new GameData(this);	
	scripter.call("onGameInit", (LuaObject*)this);
	showMainMenu();
}

void Game::registerTestScene(const char * scene)
{
	sceneNames.push_back(scene);	
}

void Game::loadTestScene(const char * scene)
{
	scripter.call("LoadSceneByName",scene);
	//worldPause(false);
}

void Game::onRender()
{
	Application::onRender();
}

void Game::onUpdate()
{
	Application::onUpdate();

	if( newActive != active )
	{
		if( active )
		{
			active->show(false);
			active->detach();
			active = NULL;
		}
				
		active = newActive;
		guiRoot->insert(active);

		if(active)
			active->show(true);		
	}
}

void Game::showMainMenu()
{	
	makeActive(new MenuWindow(this));
}

void Game::showHangar()
{
	makeActive(new HangarWindow(this));
}

void Game::makeActive(GUI::Object * object)
{
	if(active == object)
		return;	
	newActive = object;
}

///////////////////////////////////////////////////////////////////////////////////
ShipBlueprint::ShipBlueprint()
{
	blocks = NULL;	
	devices = NULL;	
	tileSize = 50;
	reset();
}

ShipBlueprint::~ShipBlueprint()
{
	reset();
}

size_t ShipBlueprint::findBlock( int x, int y, SharedPtr<GameData> data )
{
	for( Block * block = blocks; block < blocks + blocksCount; ++block)
	{
		TileSectionDesc & desc = data->sections[block->tileType];

		if( block->x >= x && block->y >= y && block->x + desc.sizeX < x && block->y + desc.sizeY < desc.sizeY)
			return block - blocks;
	}
	return -1;
}

bool ShipBlueprint::addBlock(ShipBlueprint::Block * source)
{
	blocks = (Block*)realloc(blocks, sizeof(Block)*(blocksCount + 1));
	if( blocks == NULL )
	{
		reset();
		std::_Xoverflow_error("ShipBlueprint::addBlock");
	}
	memcpy(blocks + blocksCount, source, sizeof(Block));
	blocksCount++;
	return true;
}

void ShipBlueprint::copy(ShipBlueprint * source, bool suffix)
{
	reset();
	blocksCount = source->blocksCount;	
	if( blocksCount > 0 )
	{
		blocks = (Block*)malloc(sizeof(Block)*blocksCount);	
		memcpy(blocks, source->blocks, sizeof(Block) * blocksCount);			
	}
	devicesCount = source->devicesCount;
	if( devicesCount > 0 )
	{
		devices = (Device*)malloc(sizeof(Block)*devicesCount);
		memcpy(devices, source->devices, sizeof(Device) * devicesCount);
	}
	strcpy_s(name, sizeof(name),source->name);	
}

void ShipBlueprint::load(IO::StreamIn &stream)
{
	reset();
	unsigned short size = 0;
	stream.read(size);
	blocksCount = size;
	if( blocksCount > 0 )
	{
		blocks = new Block[blocksCount];
		stream.read(blocks, sizeof(Block) * blocksCount);
	}
	stream.read(size);
	devicesCount = size;
	if( devicesCount > 0 )
	{
		devices = new Device[devicesCount];
		stream.read(devices, sizeof(Device) * devicesCount);
	}
	// TODO: implement name loading
}

void ShipBlueprint::save(IO::StreamOut & stream)
{
	stream.write((unsigned short)blocksCount);
	if(blocksCount > 0)
		stream.write(blocks, sizeof(Block) * blocksCount);
	stream.write((unsigned short)devicesCount);
	if(devicesCount > 0)
		stream.write(devices, sizeof(Device) * devicesCount);	
	// TODO: implement name saving
}

void ShipBlueprint::reset()
{
	if( blocks != NULL )
		free(blocks);
	blocks = NULL;
	blocksCount = 0;
	if( devices != NULL )
		free(devices);
	devices = NULL;
	devicesCount = NULL;
	strcpy_s(name, sizeof(name),"Enterprise XIV");
}

hgeRect ShipBlueprint::getBounds(SharedPtr<GameData> data) const
{
	hgeRect result;
	bool first = true;
	for( Block * block = blocks; block < blocks + blocksCount; ++block)
	{
		TileSectionDesc & desc = data->sections[block->tileType];
		if( first )
		{
			result.x1 = block->x;
			result.y1 = block->y;
			result.x2 = block->x + desc.sizeX;
			result.y2 = block->y + desc.sizeY;
			first = false;
		}
		if( block->x < result.x1 )
			result.x1 = block->x;
		if( block->y < result.y1 )
			result.y1 = block->y;
		if( block->x + desc.sizeX > result.x2 )
			result.x2 = block->x + desc.sizeX;
		if( block->y + desc.sizeY >= result.y2 )
			result.y2 = block->y + desc.sizeY;
		//&& block->y >= y && block->x + desc.sizeX < x && block->y + desc.sizeY < desc.sizeY)
			//return block - blocks;
	}
	return result;
} 
