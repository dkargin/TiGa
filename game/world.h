#ifndef _WORLD_H_
#define _WORLD_H_

#include "objectManager.h"
//#include "netLowHigh.h"
//#include "network.h"
#include "mapBuilderImage.h"
#include "mapBuilderQuad.h"

typedef b2Body Solid;
class Draw;
class ObjectManager;

//typedef NetCmd::Base *PNetCmd;

struct CursorInfo
{
	vec2f screenPos;		/// cursor position
	vec2f worldPos;			/// cursor world position
	GameObjectPtr object;	/// object under the cursor
};

enum KeyEventType
{
	NoEvent = 0,
	KeyDown = 1,
	KeyUp = -1,
};

class World;
struct Level
{
	struct Tile
	{
		Solid * solid;
		FxEffect * effects;
		int flags;
	};
	
	// describe wall
	struct Wall
	{
		size_t x0, y0, x1, y1;
	};

	enum WallMode
	{
		Transparent,
		WallBlocks,	// create solid block for each nonempty tile
		WallEdges,	// generate solids from wall edges. 
	};

	// basic level parameters
	std::string mapName;			
	float cellSize;
	WallMode wallMode;
	World & world;
	// derrived level parameters
	Solid * body;									/// solid root
	Raster<Tile> blocks;							/// 
	pathProject::Builder::MapBuilderImage * pathing;/// pathCore map builder
	pathProject::MapBuilderQuad * quadder;			/// quad map builder
	FxPointer background;	

	Level(World & world);
	~Level();

	bool isSolid() const;

	int init(const char * path, float cellSize, bool solid, const Pose & pose);

	int getWall(int x,int y,int z);
	void setWall(int x,int y,int z,int type);
	void initBlocks(int w, int h);
	void clear();
	void update();
	void update(int x0,int y0,int x1,int y1);

	void saveState(IO::StreamOut &stream);
	bool loadState(IO::StreamIn &stream);
protected:
	void generateWallBlocks();
	void generateWallEdges();
};

class Core;

class World:
	public ObjectManager::Listener,
	protected b2ContactFilter, 
	protected b2ContactListener
{
public:
	b2World dynamics;
	pathProject::PathCore pathCore;

	std::string name;
	bool updateSystems;
	ObjectManager *gameObjects;
	FxPointer background;
	Core * core;
	
	HTARGET				visionPass;
	hgeSprite*			visionLayer;
	GUI::hgeFontPtr		 font;			/// font for debug layer
	/// information about cursor selection
	CursorInfo cursor;

	/// network info
	int updatePeriod;		/// number of steps before update
	int updateLeft;			/// steps left to update
	bool server;
	bool useNet;
	bool helloSent;
	_Scripter & scripter;
	/// 
	Draw *draw;

/// Level speciefic
	Level level;
public:
	World(const char *name, Core * core);
	~World();
	
	void updateMap();
	/// init world - create window and init main systems
	int init(HGE *hge,bool server);
	void initMap(const char * map, float cellSize, bool rigid, const Pose &pose);
	Solid * createWall(const vec3 &from,const vec3 &to,float width);
	/// coordinate conversion & viewport
	void world2map(float worldCoords[2],int mapCoords[2]);
	void screen2world(int screenCoords[2],float worldCoords[2]);	
	void setView(float x,float y,float angle,float scale);		
	/// process cursor
	void setCursor(FxEffect * effect);		
	/// init HGE rendering
	void initRenderer(HGE* hge);

	const ObjectManager * getObjectManager() const;
	lua_State* getVM();//{return scripter.getVM();}
	Level * getLevel() { return &level;}
		
	void update(float dt);
	/// rendering
	void renderObjects();
	void renderVision();
	void renderUI();
	void render();
	/// restore surfaces
	void restore();
	/// Callbacks
	/// executed from WndProc
	void onControl(int key,KeyEventType type,int x,int y,float wheel);
	/// executed by onHit
	virtual int processHit(GameObject *a,ObjectType typea,GameObject *b,ObjectType typeb,const vec2f &normal,const vec2f &tangent);
	/// executed from ObjectManager
	void onDie(GameObject *unit);
	void onDelete(GameObject *unit);
	/// messaging API
	/*
	void readMessages(IO::StreamIn &buffer,int client);
	void writeMessages(IO::StreamOut &buffer,int client);
	void sendCmd(int client,NetCmd::Base * cmd);
	void readCmd(IO::StreamIn &buffer,int client);
	void writeCmd(IO::StreamOut &buffer,int client);
*/
	/// collision flags
	void collisionsReset(bool val=false);
	void collisionsSet(CollisionGroup a,CollisionGroup b,bool flag);
	bool collisionsGet(CollisionGroup a,CollisionGroup b);
	/// world state
	void saveLevel(const char * file);
	void loadLevel(const char * file);
	void saveState(IO::StreamOut &stream);
	bool loadState(IO::StreamIn &stream);
	/// attachable effects manager
	FxPointer attachEffect(GameObjectPtr object, FxPointer effect);
	void clearEffects(GameObjectPtr object);
protected:
	bool collisionMap[CollisionGroup::cgMax][CollisionGroup::cgMax];
//	std::map<int,std::string> cmdMap;	// map to lua commands
//	std::list<PNetCmd> commands;

	std::multimap<GameObjectPtr, FxPointer> attachedEffects;

//	void msgBegin(IO::StreamOut &buffer,int type);
//	void msgEnd(IO::StreamOut &buffer);
//	int msgPos;
	// from Box2d contact filter
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
	// from Box2d contact listener
	void BeginContact(b2Contact* contact);
};

//typedef NetCmd::Base NetCmdBase;
//typedef NetCmd::Hello NetCmdHello;
//typedef NetCmd::TakeControl NetCmdTakeControl;

void testGameObject(GameObject * object);
// lua helper - to convert char key to VK_ value
inline int vkChar(char *key)
{
	char tmp[2]={key[0],0};
	return _strupr(tmp)[0];
}
#endif
