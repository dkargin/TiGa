%{
#include "world.h"
#include "Core.h"
#include "Draw.h"
extern World *world;
extern Core * core;
%}

enum CollisionGroup
{
	cgStaticLow,
	cgStaticHigh,
	cgFlying,
	cgBody,
	cgUnit,
	cgProjectile,
	cgShield,
	cgGhost,
	cgBorder,
	cgMax,
};

struct CursorInfo
{
	vec2f screenPos;		/// cursor position
	vec2f worldPos;			/// cursor world position
	GameObject * object;	/// object under the cursor
	//FxEffect * effect;		/// cursor effect (image)
};

class Level
{
public:
	FxPointer background;
	float cellSize;
	void initBlocks(int w,int h);
	void clear();
	void update();
	void update(int x0,int y0,int x1,int y1);
	bool isSolid() const;
};

class World
{
public:
	ObjectManager *gameObjects;
	FxEffect * background;
	//Level level;
	CursorInfo cursor;
	Draw * draw;
	bool server;

	void initMap(const char * map, float cellSize, bool rigid, const Pose &pose);
	void setCursor(FxEffect * effect);
	void updateMap();
	void screen2world(int screen[2],float world[2]);
	void world2map(float worldCoords[2],int mapCoords[2]);
	Solid * createWall(float from[3],float to[3],float width);
	/// collision flags
	void collisionsReset(bool val=false);
	void collisionsSet(CollisionGroup a,CollisionGroup b,bool flag);
	bool collisionsGet(CollisionGroup a,CollisionGroup b);

	void setView(float x,float y,float angle,float scale);
//	void sendCmd(int client,NetCmdBase * cmd);
	void update(float dt);
	void renderObjects();		
	void renderVision();
	void renderUI();

	void saveLevel(const char * file);
	void loadLevel(const char * file);
	//void startMenuStage();

	Level * getLevel();

	FxEffect * attachEffect(GameObject * object, FxEffect * effect);
	void clearEffects(GameObject * object);
};



class Draw
{		
public:
	HGE *hge;	
	HTARGET *passVision;
	FxView2 globalView;	

	bool drawDynamics;
	bool drawDevices;
	bool drawTasks;
	
	void drawObject(GameObject *gameObject);
	void draw(const VisionManager::Vision &vision);
	void draw(Unit *unit);
	void drawSprite(const Pose &pose, hgeSprite * sprite);
}; 

class Core
{
public:
	LogFILE logger;
	//bool setHGEViewport(Ihandle * handle);
	void startHGEChild(unsigned int HWND);	// run HGE as child window
	void * startHGE();	
	void runHGE();							// run HGE host	
	//void worldPause(bool pause);
	//bool worldPaused() const;
	void registerTestScene(const char * scene);

	int getScreenWidth() const;
	int getScreenHeight() const;
	
	/// cursor operation
	vec2i uiGetCursorPos(size_t id = 0);
	void uiSetCursorEffect( size_t id, SharedPtr<FxEffect> effect );
	void uiResetCursorEffect( size_t id );				// reset cursor to default
	void uiSetDefaultCursorEffect( SharedPtr<FxEffect> effect );

	%extend
	{
		SharedPtr<FxManager> getFxManager()
		{
			return $self->fxManager;
		}
		Log * getLogger()
		{
			return &($self->logger);
		}
	}
};
Core * core;

int vkChar(char *key);

int rand();
int randRange(int min,int max);

void testGameObject(GameObject * object);