#pragma once

#include "sim/objectManager.h"
#include <map>

//#include "netLowHigh.h"
//#include "network.h"

// From pathProject
//#include "mapBuilderImage.h"
//#include "mapBuilderQuad.h"

typedef b2Body Solid;

namespace sim
{
	class Draw;
}

using vec2f = Fx::vec2f;
using vec3f = Fx::vec3f;
using Pose = Fx::Pose;

using namespace sim;

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

class Level
{
public:
	struct Tile
	{
		Solid* solid = nullptr;
		Fx::EntityPtr effects;
		int flags = 0;
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
	World& world;
	// derrived level parameters
	// Solid body root
	Solid* body = nullptr;
	//Raster<Tile> blocks;							///
	//pathProject::Builder::MapBuilderImage * pathing;/// pathCore map builder
	//pathProject::MapBuilderQuad * quadder;			/// quad map builder
	Fx::EntityPtr background;

	Level(World& world);
	~Level();

	bool isSolid() const;

	int init(const char* path, float cellSize, bool solid, const Pose & pose);

	int size_x() const
	{
		return blocks_w;
	}

	int size_y() const
	{
		return blocks_h;
	}

	int getWall(int x,int y,int z);
	void setWall(int x,int y,int z,int type);
	void initBlocks(int w, int h);
	void clear();
	void update();
	void updateArea(int x0,int y0,int x1,int y1);

	void saveState(sim::StreamOut &stream);
	bool loadState(sim::StreamIn &stream);

	bool isInside(int x, int y) const;
protected:
	void generateWallBlocks();
	void generateWallEdges();

	std::vector<Tile> blocks;
	int blocks_w = 0;
	int blocks_h = 0;
};

class Game;

/// The place we play in
class World:
	public ObjectManager::Listener,
	protected b2ContactFilter,
	protected b2ContactListener
{
public:
	b2World dynamics;
	//pathProject::PathCore pathCore;

	std::string name;
	bool updateSystems = false;
	std::unique_ptr<sim::ObjectManager> gameObjects;
	Game * core;

#ifdef FIX_THIS
	HTARGET visionPass;
	hgeSprite* visionLayer;
#endif

	/// information about cursor selection
	CursorInfo cursor;

	/// network info
	int updatePeriod;		/// number of steps before update
	int updateLeft;			/// steps left to update
	bool server;
	bool useNet = false;
	bool helloSent = false;
	//
	std::unique_ptr<Draw> draw;

	// Level speciefic
	Level level;
public:
	World(const char *name, Game * core);
	~World();

	void updateMap();
	/// init world - create window and init main systems
	int initSimulation(bool server);
	void initMap(const char * map, float cellSize, bool rigid, const Pose &pose);
	Solid * createWall(const vec3f &from,const vec3f &to,float width);
	/// coordinate conversion & viewport
	void world2map(float worldCoords[2],int mapCoords[2]);
	void screen2world(int screenCoords[2],float worldCoords[2]);
	void setView(float x,float y,float angle,float scale);
	/// process cursor
	void setCursor(Fx::EntityPtr effect);
	/// init HGE rendering
	struct HGE {};
	void initRenderer(HGE* hge);

	sim::Scripter * getScripter();
	const sim::ObjectManager * getObjectManager() const;
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
	virtual int processHit(GameObject *a, ObjectType typea, GameObject *b, ObjectType typeb, const vec2f &normal, const vec2f &tangent);
	/// executed from ObjectManager
	void onDie(GameObject* unit);
	void onDelete(GameObject* unit);
	/// messaging API
	/*
	void readMessages(StreamIn &buffer,int client);
	void writeMessages(StreamOut &buffer,int client);
	void sendCmd(int client,NetCmd::Base * cmd);
	void readCmd(StreamIn &buffer,int client);
	void writeCmd(StreamOut& buffer,int client);
*/
	/// collision flags
	void collisionsReset(bool val=false);
	void collisionsSet(CollisionGroup a,CollisionGroup b,bool flag);
	bool collisionsGet(CollisionGroup a,CollisionGroup b);
	/// world state
	void saveLevel(const char * file);
	void loadLevel(const char * file);

	void saveState(StreamOut &stream);
	bool loadState(StreamIn &stream);

	/// attachable effects manager
	Fx::EntityPtr attachEffect(GameObjectPtr object, Fx::EntityPtr effect);
	void clearEffects(GameObjectPtr object);
protected:
	bool collisionMap[CollisionGroup::cgMax][CollisionGroup::cgMax];
//	std::map<int,std::string> cmdMap;	// map to lua commands
//	std::list<PNetCmd> commands;

	std::multimap<GameObjectPtr, Fx::EntityPtr> attachedEffects;

	// from Box2d contact filter
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
	// from Box2d contact listener
	void BeginContact(b2Contact* contact);
};

void testGameObject(GameObject * object);
