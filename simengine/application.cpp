#include <math.h>

#include <SDL.h>

#ifdef FUCK_THIS
#include "../LuaBox/src/iup_class.h"
#include "../LuaBox/src/iup_register.h"
#endif

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

#ifdef FUCK_THIS
#include "iuplua.h"
#include "iupgl.h"
#include "iupluagl.h"
#endif
//#include "iupluacontrols.h"
#include "application.h"

namespace sim
{

Application * core = nullptr;

typedef float Scalar;

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

Application::Application()
	:logger(stderr), scripter(), pause(false), systemInitiated(false)
{
	memset(keyState,0,sizeof(keyState));
	timeAccumulator = 0.f;
	core = this;
	LogFunction(logger);
	logger.setThreat(1);

	// init LUA virtual machine
#ifdef USE_SWIG
	initLuaWrap(scripter.getVM());
#endif
	
	scripter.errorHandler = this;

	sdlRuns = false;
	iupRuns = false;
	Surf_Display = nullptr;
	renderer = nullptr;
	window = nullptr;
	appRunning = false;
}

Application::~Application()
{
	for( size_t i = 0; i < uiGetMaxCursors(); i++)
		cursor[i].effect = NULL;
	guiRoot = NULL;
	fxManager.clearObjects();
	fxManager.clearResources();

	SDL_Quit();
#ifdef FUCK_HGE
	if(sdlRuns && hge)
	{		
		hge->System_Shutdown();
		hge->Release();
	}	
#endif
#ifdef USE_IUP
	if( iupRuns )
		IupClose();
#endif
}

void Application::initIup()
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

void Application::uiProcessMouse(float mx, float my, int state)
{
	vec2f mousePos(mx, my);
	// Process mouse picks
	GUI::ObjectPtr selected = cursor[0].selected.lock();
	
	bool findNew = false;
	if( selected )
	{
		if( selected->getRect().testPoint(mousePos[0], mousePos[1]) )
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
	{
		findNew = true;
	}
	
	/// if <selected> is not empty, we try to find another window 
	if(findNew)
	{
		GUI::ObjectPtr newSelected = selected;

		auto filter = [&newSelected, mousePos](GUI::ObjectPtr object)->bool
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
		};

		guiRoot->findObject(mousePos, false, filter);

		if( selected != newSelected )
			cursor[0].selected = newSelected;
		else
			cursor[0].selected.reset();
	}
}

void Application::uiProcessEvent()
{
	if (!guiRoot)
		return;
	// TODO: Replace it by SDL events
#ifdef FUCK_HGE
	/// Process key inputs
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
#endif
}

void Application::uiRender()
{
	Fx::RenderContext rc(&fxManager);
	// Render UI
	guiRoot->callRender(&rc, guiRoot->getRect());

	for( size_t i = 0; i < uiGetMaxCursors(); i++)	
	{
		if(cursor[i].type != Cursor::Disabled &&  cursor[i].effect != NULL)
		{
			Pose pose(cursor[i].screenPosition[0],cursor[i].screenPosition[1],0,0);
			cursor[i].effect->render(&rc, pose);
		}
	}
}

void Application::updateCursorPosition(float x, float y)
{
	for( size_t i = 0; i < uiGetMaxCursors(); i++)
	{
		if(cursor[i].type != Cursor::Disabled && cursor[i].effect != nullptr)
		{
			cursor[i].screenPosition[0] = x;
			cursor[i].screenPosition[1] = y;
		}
	}
}

void Application::uiUpdate(float dt)
{
	guiRoot->callUpdate(dt);

}

int Application::getScreenWidth() const
{
	return screenWidth;
}

int Application::getScreenHeight() const
{
	return screenHeight;
}

void Application::uiSetCursorEffect( size_t id, Fx::EntityPtr effect )
{
	assert( id < uiGetMaxCursors() && cursor[id].type != Cursor::Disabled );
	cursor[id].effect = effect;
}

void Application::uiResetCursorEffect( size_t id )
{
	assert( id < uiGetMaxCursors() && cursor[id].type != Cursor::Disabled );
	cursor[id].effect = defaultCursorEffect;
}

void Application::uiSetDefaultCursorEffect( Fx::EntityPtr effect )
{ 
	defaultCursorEffect = effect; 
}

vec2f Application::uiGetCursorPos( size_t id )
{
	assert( id < uiGetMaxCursors() && cursor[id].type != Cursor::Disabled );
	return cursor[id].screenPosition;
}

size_t Application::uiGetMaxCursors() const
{
	return MaxCursors;
}

bool Application::onScripterError(sim::Scripter& scripter,const char *error)
{
	LogFunction(logger);
	logger.line(4,"Scripter error: %s",error);
	return true;
}

void Application::executeScript( const char * file )
{
	scripter.runScript( file, false );
}

void Application::run()
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
			sdlRuns = true;
			hge->System_Start();			
		}
		updateWorld();		
	}

#endif
	iupRuns = false;

	spinSDL();
	printf("Main SDL loop ended\n");
	onExit();
}

bool Application::initSDL()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}

	window = SDL_CreateWindow("An SDL2 window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			640, 480 ,0);

	if (window == nullptr)
	{
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(renderer == nullptr)
	{
		return false;
	}

	fxManager.init();

	guiRoot.reset(new GUI::Object(Fx::Rect(0,0,0,0)));
	guiRoot->setRect(Fx::Rect(0,0, getScreenWidth(), getScreenHeight()));
	cursor[0].type = Cursor::Mouse;

	return true;
}

void Application::spinSDL()
{
	SDL_Event event;

	appRunning = true;
	while(appRunning)
	{
		while(SDL_PollEvent(&event))
		{
			dispatchEvent(event);
		}

		Fx::UpdateContext uc;


		uiProcessEvent();
		uiUpdate(uc.getDelta());

		onPreRender();

		uiRender();

		SDL_GL_SwapWindow(window);
		//OnLoop();
		//OnRender();
	}

	//OnCleanup();
}

void Application::onWindowEvent(SDL_Event& event)
{
	int windowID = event.window.windowID;
	switch(event.type)
	{
	case SDL_WINDOWEVENT_SHOWN:
		SDL_Log("Window %d shown", windowID);
		break;
	case SDL_WINDOWEVENT_HIDDEN:
		SDL_Log("Window %d hidden", windowID);
		break;
	case SDL_WINDOWEVENT_EXPOSED:
		SDL_Log("Window %d exposed", windowID);
		break;
	case SDL_WINDOWEVENT_MOVED:
		SDL_Log("Window %d moved to %d,%d", windowID, event.window.data1, event.window.data2);
		break;
	case SDL_WINDOWEVENT_MINIMIZED:
		SDL_Log("Window %d minimized", windowID);
		break;
	case SDL_WINDOWEVENT_MAXIMIZED:
		SDL_Log("Window %d maximized", windowID);
		break;
	case SDL_WINDOWEVENT_RESTORED:
		SDL_Log("Window %d restored", windowID);
		break;
	case SDL_WINDOWEVENT_RESIZED:
		SDL_Log("Window %d resized to %dx%d", windowID, event.window.data1, event.window.data2);
		break;
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		SDL_Log("Window %d size changed to %dx%d", windowID, event.window.data1, event.window.data2);
		break;
	case SDL_WINDOWEVENT_ENTER:
		SDL_Log("Mouse entered window %d", windowID);
		break;
	case SDL_WINDOWEVENT_LEAVE:
		SDL_Log("Mouse left window %d", windowID);
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:
		SDL_Log("Window %d gained keyboard focus", windowID);
		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:
		SDL_Log("Window %d lost keyboard focus", windowID);
		break;
	case SDL_WINDOWEVENT_CLOSE:
		SDL_Log("Window %d closed", windowID);
		break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
	case SDL_WINDOWEVENT_TAKE_FOCUS:
		SDL_Log("Window %d is offered a focus", windowID);
		break;
	case SDL_WINDOWEVENT_HIT_TEST:
		SDL_Log("Window %d has a special hit test", windowID);
		break;
#endif
	default:
		SDL_Log("Window %d got unknown event %d", windowID, event.window.event);
		break;
	}
}

void Application::dispatchEvent(SDL_Event& event)
{
	if (event.type == SDL_QUIT)
	{
		appRunning = false;
	}
	else if (event.type == SDL_WINDOWEVENT)
	{
		onWindowEvent(event);
	}
}

}