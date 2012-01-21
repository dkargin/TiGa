#ifndef _COMMON_H_
#define _COMMON_H_
#define NOMINMAX
#define GLUT_BUILDING_LIB
#include <tchar.h>
#include "glut.h"
#include "../../frosttools/3dmath.h"
#include "../../frosttools/frosttools.h"

struct Color{ float r, g, b, a;};


//////////////////////////////////////////////////////////////////////////////////////
// to be implemented in each demo
class BaseApp
{
	// extern GLUT functions
	friend void baseResize(int w, int h);
	friend void baseDisplay(void);
	friend void baseIdle(void);
	friend void baseMouse( int button, int state, int screenX, int screenY );
protected:
	int rootWindow;
	std::vector<Color> colorStack;
	int screenWidth, screenHeight;
public:
	typedef unsigned int Color32;
	BaseApp();
	virtual ~BaseApp();
	virtual int demoFinish();			// calls when demo finishes
	virtual int demoStart();			// calls whed demo starts. GL system already started
	virtual int demoRender();			// calls each frame
	virtual int	demoUpdate(float dt);

	virtual void onMouse(const vec2i & screenPos, int button, int state) {}
	virtual void onMouseMove( const vec2i & screenPos) {}
	vec2f screenToWorld( const vec2i & screenPos );

	void pushColor(float r, float g, float b, float a = 1.0);
	void popColor();
	
	void drawLine(const vec2f &from, const vec2f &to);
	void drawArrow(const vec2f &from,const vec2f &to, float width = 0.1f, float length = 0.2f);
	static BaseApp * gInstance;
protected:
	
};
//////////////////////////////////////////////////////////////////////////////////////
vec3 screen2world(int x,int y);	// converts screen coordinates to world
/// common functions
/// init GL system
int init_GL(const char *caption,int width,int height);
/// resize main window size
void resizeWindow(int width,int height);	
/// sets orthographic projection. screen coordinates are equal to world coordinates
void setOrtho(int wnd);
/// drawing routines
void drawPoint(const vec3 &pt,float size,int type);			// draw point

void glTransform(const Pose2 & pose);
void glTransform(const Pose2z & pose);


//void draw(const pathProject::PathSegment *seg,int count);	// draws segments path
//void draw(const pathProject::Waypoint * wp,int count);		// draws wayoints path
void drawGrid(const Geom::AABB &bounds,int width,int height);

const vec2f offsets[]=
{
	vec2f(0,-1),vec2f(1,0),vec2f(0,1),vec2f(-1,0)
};

const int reverseDir[]=
{
	2,3,0,1,	
};

typedef vec2f vec;
const int dirCount = (sizeof(offsets)/sizeof(offsets[0]));
extern float mapCellSize; 
//typedef float Flow[dirCount];
#endif