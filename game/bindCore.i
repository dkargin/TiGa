%{
#include "Core.h"
%}

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
		FxManagerPtr getFxManager()
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