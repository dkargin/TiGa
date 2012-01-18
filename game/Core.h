#ifndef _CORE_H_
#define _CORE_H_

#include <fxObjects.h>

typedef struct Ihandle_ Ihandle;

const int MaxCursors = 1;

class Core: public _Scripter::ErrorHandler
{
public:
	LogFILE logger;
	_Scripter scripter;

	bool iupRuns;
	bool hgeRuns;

	bool pause;
	bool systemInitiated;
	float timeAccumulator;

	struct Cursor
	{
		vec2f screenPosition;
		FxPointer effect;
		WeakPtr<GUI::Object> selected;
		enum Type
		{
			Disabled,
			Mouse,
			Touch,
		}type;
	}cursor[MaxCursors];

	FxPointer defaultCursorEffect;	

	bool keyState[255];	

	Core(const char * baseScript);
	virtual ~Core();
	/// system startup
	void initIup();
	void runSystem();	// run main script
	void startHGEChild(unsigned int HWND);	// run HGE as child window
	bool setHGEViewport(Ihandle * handle);
	Ihandle * startHGE(int width = 800, int height = 600, bool windowed = true);				// init HGE as host
	void runHGE();							// run HGE host, called from sctipts
	void exitHGE();
	/// cursor operation
	vec2i uiGetCursorPos(size_t id = 0);
	void uiSetCursorEffect( size_t id, FxEffect * effect );
	void uiResetCursorEffect( size_t id );				// reset cursor to default
	void uiSetDefaultCursorEffect( FxEffect * effect );
	size_t uiGetMaxCursors() const;
	/// world control
	virtual void createWorld();						// create world
	virtual void worldPause(bool pause);
	virtual bool worldPaused() const;	
	virtual void updateWorld();
	/// screen info
	int getScreenWidth() const;
	int getScreenHeight() const;

	// gui
	SharedPtr<GUI::Object> guiRoot;	// root gui object	
	hgeFont *font;

	FxManagerPtr fxManager;
	
	/// general callbacks, mostly from HGE
	virtual void onPreRender(){}
	virtual void onRender();
	virtual void onUpdate();	
	virtual void onExit(){}
	/// test scene control. Seems to be deprecated
	virtual void registerTestScene(const char * scene);	
	virtual void loadTestScene(const char * scene);
protected:	
	void uiProcessEvent();
	void uiRender();
	void uiUpdate(float dt);
	// from Scripter error listener
	bool onScripterError(_Scripter &scripter,const char *error);
private:
	// HGE event handlers
	static bool FrameFunc();
	static bool RestoreFunc();
	static bool RenderFunc();
	HGE * BasicHGEInit();
	void initFxManager();
	void releaseFxManager();
};

extern Core * core;

#endif