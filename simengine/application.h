#pragma once

#include "fx/fxobjects.h"
#include "fx/fxmanager.h"
#include "gui/guictrls.h"

#include "logger.hpp"
#include "sim/basetypes.h"

typedef struct Ihandle_ Ihandle;

const int MaxCursors = 1;

// Some forward declarations from SDL
class SDL_Surface;
class SDL_Window;
class SDL_Renderer;
union SDL_Event;

namespace sim
{

/// Mouse state
struct MouseState
{
	bool moved;
	int wheel;
	vec2f position;
};

// We gather all frame events in this structure
struct FrameEvents
{
	bool windowVisible = true;
	bool windowMoved = false;
	bool windowResized = false;
	Fx::IntRect windowGeometry;
	// Something mouse-specific happened
	bool mouseMoved = false;
	// Something keyboard-specific happened
	bool keysChanged = false;
	// Flag shows that application wants to exit
	bool exit = false;
	// Total number of events encountered
	int eventCounter = 0;
	// Updated mouse state
	std::vector<MouseState> mouseState;
};
/**
 * Base application class
 * Deals with main window and wraps input
 */
class Application:
		public sim::Scripter::ErrorHandler,
		public sim::LuaObject
{
public:
	frosttools::LogFILE logger;
	sim::Scripter scripter;

	bool pause;
	bool systemInitiated;
	float timeAccumulator;

	struct Cursor
	{
		sim::vec2f screenPosition;
		Fx::EntityPtr effect;
		// Selected gui object
		GUI::ObjectWPtr selected;
		enum Type
		{
			Disabled,
			Mouse,
			Touch,
		}type;
	};

	Fx::EntityPtr defaultCursorEffect;

	// Keyboard state
	bool keyState[255];	

	Application();
	virtual ~Application();

	virtual bool initSDL();

	/// system startup
	void executeScript(const char * file);
	void run();	// run main script
#ifdef FUCK_HGE
	void startHGEChild(unsigned int HWND);	// run HGE as child window
	bool setHGEViewport(Ihandle * handle);
	Ihandle * startHGE(int width = 800, int height = 600, bool windowed = true);				// init HGE as host

	//
	void runHGE();							// run HGE host, called from sctipts
	void exitHGE();
#endif
	/// cursor operation
	Fx::vec2f uiGetCursorPos(size_t id=0);
	void uiSetCursorEffect( size_t id, Fx::EntityPtr effect );
	void uiResetCursorEffect( size_t id );				// reset cursor to default
	void uiSetDefaultCursorEffect( Fx::EntityPtr effect );
	size_t uiGetMaxCursors() const;

	// root gui object
	GUI::ObjectPtr guiRoot;
	GUI::FontPtr font;

	Fx::FxManager fxManager;
	
	/// general callbacks, mostly from HGE
	virtual void onPreRender(){}
	virtual void onExit() {}
	virtual void onRestore() {}

	sim::Scripter * getScripter() { return &scripter;}

protected:
	// Gathers input state from HGE. Should be refactored to SDL
	void uiProcessEvent();
	void uiProcessMouse(float mx, float my, int state);
	void uiRender();
	void uiUpdate(float dt);
	// from Scripter error listener
	bool onScripterError(Scripter& scripter,const char *error);

	// Update cursor's screen position
	void updateCursorPosition(float x, float y);

protected:
	bool sdlRuns = false;

	// SDL stuff
	SDL_Surface* Surf_Display = nullptr;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	Cursor cursor[MaxCursors];

	// Control flag for spinning application event loop
	bool appRunning = false;
	std::vector<MouseState> mouseState, newMouseState;

	Fx::IntRect windowGeometry;
	// Run SDL event loop
	void spinSDL();
};

extern Application * core;

int vkChar(char *key);

} // namespace sim
