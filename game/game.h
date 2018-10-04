#ifndef _GAME_H_
#define _GAME_H_ 

#include <simengine/application.h>
#include <simengine/fx/fxobjects.h>
#include <simengine/gui/guibase.h>

class MenuWindow;
class Game;
class World;
class GameData;

struct ShipBlueprint;

class Game : public sim::Application
{
public:
	Game();
	~Game();

	std::list<std::string> sceneNames;

	//SharedPtr<MenuWindow> mainMenu;	
	//SharedPtr<ShipyardWindow> shipyard;
	//SharedPtr<HangarWindow> hangar;

	GUI::FontPtr		 font;						///< font for debug layer
	std::shared_ptr<GameData> gameData;
	//World * world;
	GUI::ObjectPtr active, newActive;
 
	virtual void onRender();
	virtual void onUpdate();
	/// test scene control. Seems to be deprecated
	virtual void registerTestScene(const char * scene);	
	virtual void loadTestScene(const char * scene);
	/// init game data and basic UI
	virtual void initGame();
	/// show specific frame
	//void showGameplay();
	void showHangar();
	void showMainMenu();
	void makeActive(GUI::ObjectPtr active);
	void popGameState();
	const char * luaName()const { return "Game";}

	/// shared data for different game frames
	size_t selectedBlueprintId;			// blueprint index
	ShipBlueprint * selectedBlueprint;	// blueprint to be edited on the shipyard
protected:
	
};


//namespace Style
//{
	// common UI style constants
	const float buttonWidth = 100;
	const float buttonHeight = 20;
	const float buttonSpacing = 5;

	const Fx::FxRawColor clrButtons = Fx::makeARGB(255,128,128,128);
//}

struct TileSectionDesc
{
	size_t spriteId;
	unsigned char sizeX, sizeY; 

	int type;
	size_t strNameId;			// string id for section name
	size_t strDescriptionId;	// string id for section description
	unsigned short mass;
	unsigned short health;
	unsigned short cost;
	//TileSectionDesc();
};

struct DeviceSectionDesc
{
	unsigned char sizeX, sizeY;
	int type;
	size_t strNameId;			// string id for section name
	size_t strDescriptionId;	// string id for section description

	unsigned short mass;
	unsigned short health;
	unsigned short cost;
};


// storage for Effects
class FxStorage
{
public:
	size_t addEffect(Fx::EntityPtr ptr);
	size_t addNamedEffect(Fx::EntityPtr ptr, const char * name);
	Fx::EntityPtr findByString(size_t id);
	Fx::EntityPtr findById(size_t id);
protected:
	std::map<std::string, size_t> names;
	std::vector<Fx::EntityPtr> effects;				/// cached sprites
};
/// Provides storage for:
/// - resources
/// - tile description
/// - blueprints
class GameData
{
public:
	GameData(Game * game);
	~GameData();

	ShipBlueprint * blueprints;
	size_t blueprintsCount;

	/// fast access
	Fx::EntityPtr getSectionSprite(size_t sectionId);
	Fx::EntityPtr getSprite(size_t spriteId);
	/// resources
	std::vector<TileSectionDesc> sections;
	std::vector<Fx::EntityPtr> sprites;				/// cached sprites
	std::vector<std::string> strings;				/// string resources
};

/// Describes ship layout
struct ShipBlueprint
{
	struct Block
	{
		short x, y;					// tile coordinates
		short direction;			// tile direction
		unsigned short tileType;	
	};

	std::vector<Block> blocks;
	
	struct Device
	{
		short x, y;					// tile coordinates
		short angle;				// device orientation in degrees
		unsigned short deviceType;	// device type
	};

	std::vector<Device> devices;

	float tileSize;					// size of tile, in pixels ?

	// ship name
	std::string name;

	ShipBlueprint();
	~ShipBlueprint();

	size_t findBlock(int x, int y, const GameData& gameData);	// find first block
	bool addBlock(Block * source);

	Fx::Rect getBounds(const GameData& gameData) const;	/// get cells bounding rect
	
	unsigned int getTotalCost() const;
	unsigned int getTotalMass() const;

	/// remove all blueprint contents
	void reset();	
	/// copy all data from <source>
	void copy(ShipBlueprint * source, bool suffix = false);	

	/// serialisation
#ifdef FIX_THIS
	void load(IO::StreamIn & stream);
	void save(IO::StreamOut & stream);
#endif
};

void GenerateShipGraphics(Fx::EntityPtr root, ShipBlueprint * blueprint, Game * game);

#endif
