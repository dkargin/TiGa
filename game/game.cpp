#include "world.h"
#include "game.h"

#include "frameMainMenu.h"
#include "frameHangar.h"

int hgeMain()
{
	Game game;
	game.executeScript("./script/startup_Game.lua");
	game.font = GUI::FontPtr(new Fx::Font("./data/font1.fnt"));
	game.initGame();
	game.run();
	return 0;
}

extern "C" int luaopen_TiGa(lua_State* L);


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
	gameData = std::make_shared<GameData>(this);
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
	makeActive(std::make_shared<MenuWindow>(this));
}

void Game::showHangar()
{
	makeActive(std::make_shared<HangarWindow>(this));
}

void Game::makeActive(GUI::ObjectPtr object)
{
	if(active == object)
		return;
	newActive = object;
}

///////////////////////////////////////////////////////////////////////////////////
ShipBlueprint::ShipBlueprint()
{
	tileSize = 50;
	reset();
}

ShipBlueprint::~ShipBlueprint()
{
	reset();
}

size_t ShipBlueprint::findBlock( int x, int y, const GameData& data )
{
	//for(Block * block = blocks; block < blocks + blocksCount; ++block)
	for(int i = 0; i < blocks.size(); i++)
	{
		const Block& block = blocks[i];
		const TileSectionDesc& desc = data.sections[block.tileType];

		if( block.x >= x && block.y >= y && block.x + desc.sizeX < x && block.y + desc.sizeY < desc.sizeY)
			return i;
	}
	return -1;
}

bool ShipBlueprint::addBlock(ShipBlueprint::Block* source)
{
	blocks.push_back(*source);
	return true;
}

void ShipBlueprint::copy(ShipBlueprint* source, bool suffix)
{
	reset();

	blocks = source->blocks;
	devices = source->devices;
	name = source->name;
}

#ifdef FIX_THIS
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
#endif

void ShipBlueprint::reset()
{
	blocks.clear();
	devices.clear();
	name = "Enterprise XIV";
}

Fx::Rect ShipBlueprint::getBounds(const GameData& data) const
{
	Fx::Rect result;
	bool first = true;

	for( const Block& block: blocks)
	{
		const TileSectionDesc & desc = data.sections[block.tileType];
		if( first )
		{
			result.x1 = block.x;
			result.y1 = block.y;
			result.x2 = block.x + desc.sizeX;
			result.y2 = block.y + desc.sizeY;
			first = false;
		}
		if( block.x < result.x1 )
			result.x1 = block.x;
		if( block.y < result.y1 )
			result.y1 = block.y;
		if( block.x + desc.sizeX > result.x2 )
			result.x2 = block.x + desc.sizeX;
		if( block.y + desc.sizeY >= result.y2 )
			result.y2 = block.y + desc.sizeY;
		//&& block->y >= y && block->x + desc.sizeX < x && block->y + desc.sizeY < desc.sizeY)
			//return block - blocks;
	}
	return result;
}
