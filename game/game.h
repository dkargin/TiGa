#ifndef _GAME_H_
#define _GAME_H_ 

#include "Core.h"

class MenuWindow;
class Game;
class World;
class GameData;

struct ShipBlueprint;

class Game : public Core
{
public:
	Game();
	~Game();

	std::list<std::string> sceneNames;

	//SharedPtr<MenuWindow> mainMenu;	
	//SharedPtr<ShipyardWindow> shipyard;
	//SharedPtr<HangarWindow> hangar;

	GUI::FontPtr		 font;				/// font for debug layer
	SharedPtr<GameData> gameData;
	//World * world;
	SharedPtr<GUI::Object> active, newActive;
 
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
	void makeActive(GUI::Object * active);
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

	const DWORD clrButtons = ARGB(255,128,128,128);
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

/// Provides storage for:
/// - resources
/// - tile description
/// - blueprints
class GameData : public Referenced
{
public:
	GameData(Game * game);
	~GameData();

	ShipBlueprint * blueprints;
	size_t blueprintsCount;

	/// fast access
	FxEffect::Pointer getSectionSprite(size_t sectionId);	
	FxEffect::Pointer getSprite(size_t spriteId);
	/// resources
	std::vector<TileSectionDesc> sections;
	std::vector<FxEffect::Pointer> sprites;				/// cached sprites	
	std::vector<std::string> strings;				/// string resources
};

/// Describes ship layout
struct ShipBlueprint : public Referenced
{
	struct Block
	{
		short x, y;					// tile coordinates
		short direction;			// tile direction
		unsigned short tileType;	
	};
	Block * blocks;
	size_t blocksCount;

	struct Device
	{
		short x, y;					// tile coordinates
		short angle;				// device orientation in degrees
		unsigned short deviceType;	// device type
	};
	Device * devices;
	size_t devicesCount;

	char name[255];				// ship name

	ShipBlueprint();
	~ShipBlueprint();

	size_t findBlock(int x, int y, SharedPtr<GameData> gameData);	// find first block
	bool addBlock(Block * source);

	hgeRect getBounds(SharedPtr<GameData> gameData) const;	/// get cells bounding rect
	
	unsigned int getTotalCost() const;
	unsigned int getTotalMass() const;
	/// remove all blueprint contents
	void reset();	
	/// copy all data from <source>
	void copy(ShipBlueprint * source, bool suffix = false);	
	/// serialisation
	void load(IO::StreamIn & stream);
	void save(IO::StreamOut & stream);
};
#endif