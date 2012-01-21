#include "stdafx.h"

#include <math.h>

//#include "world.h"
#include "iup.h"
#include "../LuaBox/src/iup_class.h"
#include "../LuaBox/src/iup_register.h"

#ifdef USE_IUP
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "lua51static_mt.lib")
#pragma comment(lib, "iupcore.lib")
#pragma comment(lib, "iupim.lib")
#pragma comment(lib, "iup.lib")
#pragma comment(lib, "iupimglib.lib")
#pragma comment(lib, "iupgl.lib")
#pragma comment(lib, "iupcontrols.lib")
#pragma comment(lib, "iuplua51.lib")
#pragma comment(lib, "iupluagl51.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "iupluacontrols51.lib")
#endif
#include "iuplua.h"
#include "iupgl.h"
#include "iupluagl.h"
//#include "iupluacontrols.h"
#include "Core.h"

int regStore(lua_State *l);	// store top stack object in lua registry
int regGet(lua_State *l);	// get object from lua registry
int regFree(lua_State *l);	// free lua registry object
/*
Ihandle * IupHGE(Ihandle * child);
void iupRegisterHGE();
*/
Core * core = NULL;
//World * world = NULL;
HGE * hge = 0;

typedef float Scalar;

// lua helper - to convert char key to VK_ value
int vkChar(char *key)
{
	char tmp[2]={key[0],0};
	return _strupr(tmp)[0];
}

inline Scalar NormalizeAngle( Scalar degrees )
{
	float integral;
	float fractional = modff( degrees, &integral );
	float normalizedAngle = (float)( (int)( fabs( integral ) ) % 360 );

	if( degrees < 0 )
		return 360.f - normalizedAngle + fractional;
	else return normalizedAngle + fractional;
}
// returns minimal angular distance
float AngleMinDelta(float a, float b)
{
	a = NormalizeAngle(a);
	b = NormalizeAngle(b);
	float delta = b - a;
	if( delta > 180.f)
		return -360.f + delta;
	if( delta <-180.f)
		return 360.f + delta;
	return delta;
}

float screenWidth = 800,screenHeight = 600;

//////////////////////////////////////////////////////////////////////////////////////////

void initLuaWrap( lua_State * l );

Core::Core()
	:logger(stderr), scripter(), pause(false), systemInitiated(false)
{
	memset(keyState,0,sizeof(keyState));
	timeAccumulator = 0.f;
	core = this;
	LogFunction(logger);
	//g_logger = &logger;

	logger.setThreat(1);
	// init LUA virtual machine
	initLuaWrap(scripter.getVM());
	
	scripter.errorHandler = this;

	hgeRuns = false;
	iupRuns = false;
}

Core::~Core()
{
	releaseFxManager();
	if(hgeRuns && hge)
	{		
		hge->System_Shutdown();
		hge->Release();
	}	
#ifdef USE_IUP
	if( iupRuns )
		IupClose();
#endif
}

void Core::initIup()
{
	iupRuns = false;
#ifdef USE_IUP
	iuplua_open(scripter.getVM());
	iupgllua_open(scripter.getVM());

	IupImageLibOpen();
	//iupRegisterHGE();
	//iupcontrolslua_open(l);
	//cdlua_open(L);
	//cdluaiup_open(L);
	//cdInitContextPlus();
	//iupcontrolslua_open(L);	
#endif	
	iupRuns = true;
}
// This function will be called by HGE when
// render targets were lost and have been just created
// again. We use it here to update the render
// target's texture handle that changes during recreation.
bool Core::RestoreFunc()
{
	core->onRestore();
	return false;
}

void Core::uiProcessEvent()
{
	vec2f mousePos;
	hge->Input_GetMousePos(mousePos + 0, mousePos + 1);
	//guiRoot->callMouseMove(0, mousePos);
	//WeakPtr<GUI::Object> selected;
	
	WeakPtr<GUI::Object> selected = cursor[0].selected;
	
	bool findNew = false;
	if( selected )
	{
		if( selected->getRect().TestPoint(mousePos[0], mousePos[1]) )
		{
			if(!selected->onMouseMove(0, mousePos, GUI::Object::MoveRemain))
				findNew = true;	// reset selection, so we would not change active window
		}
		else
		{
			selected->onMouseMove(0, mousePos, GUI::Object::MoveLeave);
			findNew = true;
			// find another window to notify
		}
	}
	else
		findNew = true;
	
	/// if <selected> is not empty, we try to find another window 
	if(findNew)
	{
		GUI::Object * newSelected = selected;
		guiRoot->findObject(mousePos, false, [&newSelected, mousePos](GUI::Object * object)->bool
		{
			if( object != newSelected)
			{
				if(!object->onMouseMove(0, mousePos, GUI::Object::MoveEnter))
					return false;	// continue search
			}
			else 
				return false;
			newSelected = object;
			return true;			// stop search
		});
		if( cursor[0].selected != newSelected )
			cursor[0].selected = newSelected;
		else
			cursor[0].selected = NULL;
	}
	
	for(size_t i = 0; i < 255; ++i)
	{		
		bool newState = hge->Input_GetKeyState(i);
		
		if(newState != keyState[i])
		{
			
			int s = newState ? 1 : 0;
			switch(i)
			{
			case HGEK_LBUTTON:
				guiRoot->callMouse(0, 0, s, mousePos);
				break;
			case HGEK_RBUTTON:
				guiRoot->callMouse(0, 1, s, mousePos);
				break;
			case HGEK_MBUTTON:
				guiRoot->callMouse(0, 2, s, mousePos);
				break;
			default:
				break;
			}
			keyState[i] = newState;
			//world->onControl(i, newState ? KeyEventType::KeyDown : KeyEventType::KeyUp, mousePos[0], mousePos[1], 0);			
		}		
	}	
}

void Core::uiRender()
{
	guiRoot->callRender(guiRoot->getRect());
	// show all avialable cursors
	for( size_t i = 0; i < uiGetMaxCursors(); i++)	
		if(cursor[i].type != Cursor::Disabled &&  cursor[i].effect != NULL)
			cursor[i].effect->render(fxManager.get(), Pose2z(cursor[i].screenPosition[0],cursor[i].screenPosition[1],0,0));	
}

void Core::uiUpdate(float dt)
{
	guiRoot->callUpdate(dt);
	for( size_t i = 0; i < uiGetMaxCursors(); i++)	
		if(cursor[i].type != Cursor::Disabled &&  cursor[i].effect != NULL)
		{
			vec2f & mousePos = cursor[i].screenPosition;
			hge->Input_GetMousePos(mousePos + 0, mousePos + 1);
		}
}

int Core::getScreenWidth() const
{
	return hge->System_GetState(HGE_SCREENWIDTH);
}

int Core::getScreenHeight() const
{
	return hge->System_GetState(HGE_SCREENHEIGHT);
}

void Core::uiSetCursorEffect( size_t id, FxEffect * effect )
{
	assert( id < uiGetMaxCursors() && cursor[id].type != Cursor::Disabled );
	cursor[id].effect = effect;
}

void Core::uiResetCursorEffect( size_t id )
{
	assert( id < uiGetMaxCursors() && cursor[id].type != Cursor::Disabled );
	cursor[id].effect = defaultCursorEffect;
}

void Core::uiSetDefaultCursorEffect( FxEffect * effect ) 
{ 
	defaultCursorEffect = effect; 
}

vec2i Core::uiGetCursorPos( size_t id )
{
	assert( id < uiGetMaxCursors() && cursor[id].type != Cursor::Disabled );
	return cursor[id].screenPosition;
}

size_t Core::uiGetMaxCursors() const
{
	return MaxCursors;
}

void Core::exitHGE()
{
	hge->System_Shutdown();
}

void Core::onUpdate()
{
	if( true )
	{
		float dt = hge->Timer_GetDelta();
		core->uiProcessEvent();
		core->uiUpdate(dt);
	}
#ifdef USE_IUP
	if(iupRuns)
		IupLoopStep();
#endif
}

void Core::onRender()
{		
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	uiRender();
	hge->Gfx_EndScene();
}

bool Core::FrameFunc()
{
	core->onUpdate();
	return false;
}

bool Core::RenderFunc()
{
	core->onRender();
	return false;
}

bool Core::onScripterError(_Scripter &scripter,const char *error)
{
	LogFunction(logger);
	_RPT1(0,"Scripter error: \n %s \n",error);
	logger.line(4,"Scripter error: %s",error);
	return true;
}

//RECT iupGetRect(Ihandle * handle);
bool Core::setHGEViewport(Ihandle * handle)
{
	//RECT rc = iupGetRect(handle);
	//hge->Gfx_SetViewport(rc.left, rc.top, (rc.right - rc.left)/2, (rc.bottom - rc.top)/2);
	//hge->Gfx_SetClipping(rc.left, rc.top, (rc.right - rc.left)/2, (rc.bottom - rc.top)/2);
	return true;
}

void Core::runHGE()
{
	assert(systemInitiated);
	hgeRuns = true;
	hge->System_Start();		
}
/*
void Core::createWorld()
{
	assert(hge);	
}
*/
void Core::executeScript( const char * file )
{
	scripter.runScript( file, false );
}

void Core::run()
{
	bool _isDataInitialized = false;
#ifdef USE_IUP
	while(iupRuns && (IupLoopStep() != IUP_CLOSE))
	{
		iupRuns = true;
		if(hge)
		{
			if (!_isDataInitialized && hge->System_GetState(HGE_HWND))
			{
				// hge is ready
				_isDataInitialized = true;
			} 
			hgeRuns = true;
			hge->System_Start();			
		}
		updateWorld();		
	}
#endif
	iupRuns = false;
	printf("Main IUP loop ended\n");
	runHGE();
	onExit();
}
/*
void Core::updateWorld()
{	
	const float updateDeltaMS = 1.0f/60.0f;
	if(!hge)
		return;

	float mx, my;
	hge->Input_GetMousePos(&mx, &my);
	if(world)
	{
		world->cursor.screenPos[0] = mx;
		world->cursor.screenPos[1] = my;
	}
	float dt = hge->Timer_GetDelta();
	if(world && !worldPaused())
	{
		timeAccumulator += dt;
		if(timeAccumulator >= updateDeltaMS)
		{
			world->update(updateDeltaMS);
			timeAccumulator -= updateDeltaMS;
		}		
	}
	scripter.call("onUpdate",dt);
}
*/

HGE * Core::getHGE()
{
	return hge;
}

void Core::initFxManager()
{
	if( fxManager == NULL )
	{
		fxManager = FxManagerPtr(new FxManager(&logger));
		fxManager->init(hge);
		guiRoot = new GUI::Object(hgeRect(0,0,0,0));
		guiRoot->setRect(hgeRect(0,0, getScreenWidth(), getScreenHeight()));
		cursor[0].type = Cursor::Mouse;
	}
}

void Core::releaseFxManager()
{
	for( size_t i = 0; i < uiGetMaxCursors(); i++)	
		cursor[i].effect = NULL;
	guiRoot = NULL;
	fxManager->clearObjects();
	fxManager->clearResources();
}

HGE * Core::BasicHGEInit()
{
	hge = hgeCreate(HGE_VERSION);

	if(!hge)
		throw(std::exception("Cannot create HGE instance"));

	hge->System_SetState(HGE_LOGFILE, "TiGa.log");
	hge->System_SetState(HGE_FRAMEFUNC, Core::FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, Core::RenderFunc);
	hge->System_SetState(HGE_GFXRESTOREFUNC, Core::RestoreFunc);
	hge->System_SetState(HGE_TITLE, "TiGa");
	hge->System_SetState(HGE_FPS, 60);
	hge->System_SetState(HGE_SCREENBPP, 32);
	
	initFxManager();
	this->hgeRuns = true;
	return hge;
}

Ihandle * Core::startHGE(int width, int height, bool windowed)
{
	assert(!hge);

	hge = BasicHGEInit();

	screenWidth = width;
	screenHeight = height;
	
	hge->System_SetState(HGE_WINDOWED, windowed);
	hge->System_SetState(HGE_SCREENWIDTH, width);
	hge->System_SetState(HGE_SCREENHEIGHT, height);	
	
	systemInitiated = hge->System_Initiate();
	
	return NULL;
}

void Core::startHGEChild(unsigned int wnd)
{
	assert(hge == NULL);
	hge = BasicHGEInit();
	
	hge->System_SetState(HGE_GUEST, true);
	hge->System_SetState(hgeHwndState::HGE_HWNDPARENT,(HWND)wnd);
	hge->System_SetState(HGE_WINDOWED, true);
		
	if(hge->System_Initiate()) 
	{	
		hge->System_Start();
	}
	else
		throw(std::exception("Cannot start HGE"));
}