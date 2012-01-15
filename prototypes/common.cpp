#include "common.h"

#pragma comment(lib, "glut32.lib")
///////////////////////////////////////////////////////////////
unsigned int frames=0;
int window=-1;

BaseApp * BaseApp::gInstance = NULL;
BaseApp::BaseApp()
	:rootWindow(-1)
{
	assert( gInstance == NULL );
	gInstance = this;
}
BaseApp::~BaseApp()
{
	assert( gInstance == this );
	gInstance = NULL;
}
// calls when demo finishes
int BaseApp::demoFinish()
{
	return 0;
}				
// calls whed demo starts. GL system already started
int BaseApp::demoStart() 
{
	return 0;
}				
// calls each frame
int BaseApp::demoRender() 
{
	return 0;
}				
int	BaseApp::demoUpdate(float dt) 
{
	return 0;
}

////////////////////////////////////////////////////////////////
void BaseApp::pushColor(float r, float g, float b, float a)
{
	glColor4f(r,g,b,a);
	Color color = {r,g,b,a};
	colorStack.push_back(color);
}

void BaseApp::popColor()
{	
	colorStack.pop_back();
	if( !colorStack.empty() )
	{
		Color color = colorStack.back();
		glColor4fv((float*)&color);
	}
}

vec2f BaseApp::screenToWorld( const vec2i & screen )
{
	return vec2f( screen[0] - screenWidth/2, screen[1] - screenHeight/2 );
}

void BaseApp::drawLine(const vec2f &from, const vec2f &to)
{
	glBegin(GL_LINES);
	glVertex2fv( from );
	glVertex2fv( to );
	glEnd();
}

void BaseApp::drawArrow(const vec2f &from,const vec2f &to, float width, float length)
{
	vec2f dir = to - from;
	vec2f left(-dir[1],dir[0]);
	glBegin(GL_LINES);
	glVertex2fv( to );
	glVertex2fv( from );
	glVertex2fv( to - dir*length - left*width );
	glVertex2fv( to );
	glVertex2fv( to - dir*length + left*width );
	glVertex2fv( to );
	drawLine( to, from );
	glEnd();	
}
////////////////////////////////////////////////////////////////
enum DemoState
{
	Invalid,
	Init,
	Run,
}demoState = Invalid;

static void baseDisplay(void)
{
	//glClearColor(0.502f, 0.502f, 0.753f, 1.0f); //Background color
	glClearColor(1.f, 1.f, 1.f, 1.0f); //Background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers	
	
	setOrtho(window);
	if( BaseApp::gInstance != NULL )
		BaseApp::gInstance->demoRender();
	frames++;
	glutSwapBuffers();
}

static void baseResize(int w, int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if( BaseApp::gInstance != NULL )
	{
		BaseApp::gInstance->screenWidth = w;
		BaseApp::gInstance->screenHeight = h;
	}
}

static void baseIdle(void)
{
	baseDisplay();
}

static void baseTimer(int value)
{
	baseDisplay();
}

static void baseKeyboard(unsigned char key, int x, int y)
{   /*
	switch(key)
	{	
	default:		
		
	}*/	
	exit(1);
}

static void baseMouse( int button, int state, int screenX, int screenY )
{
	if( demoState != Invalid && BaseApp::gInstance != NULL )
		BaseApp::gInstance->onMouse( vec2i(screenX,screenY), button, state );
}

static void baseMouseMove( int screenX, int screenY )
{
	if( demoState != Invalid && BaseApp::gInstance != NULL )
		BaseApp::gInstance->onMouseMove( vec2i(screenX,screenY) );
}

void onWndExit(int code)
{
	exit(0);
}

void onExit()
{
	if(demoState != Invalid)
	{
		if( BaseApp::gInstance != NULL )
			BaseApp::gInstance->demoFinish();	
		demoState = Invalid;
	}
}

const float deltaT = 0.066f;

void onTimer(int value)
{			
	if(BaseApp::gInstance == NULL || !BaseApp::gInstance->demoUpdate(deltaT))
	{
		exit(0);
	}
	// repeat timer
	glutTimerFunc(1000*deltaT,&onTimer,0);
}

void resizeWindow(int width,int height)
{
	glutSetWindow(window);
	glutReshapeWindow(width,height);
}

int init_GL(const char *caption,int width,int height)
{	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE /*| GLUT_DEPTH*/);
	glutInitWindowSize(width, height);
	window=__glutCreateWindowWithExit(caption,&onWndExit);
	glutDisplayFunc(baseDisplay);
	glutReshapeFunc(baseResize);
	glutMouseFunc(baseMouse);
	glutMotionFunc(baseMouseMove);
	glutPassiveMotionFunc(baseMouseMove);
	glutKeyboardFunc(baseKeyboard);
	glutIdleFunc(baseIdle);		
	glutTimerFunc(1000*deltaT,&onTimer,0);
	return window;
}

int _tmain(int argc, _TCHAR * argv[])
{	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	atexit(&onExit);	
	int mainWindow = init_GL("Waterflow Demo",400,300);
	demoState = Init;
	if(BaseApp::gInstance != NULL)
	{
		if(BaseApp::gInstance->demoStart())
		{
			demoState=Run;
			glutMainLoop();
		}
		BaseApp::gInstance->demoFinish();
	}
	glutDestroyWindow( mainWindow);
	return 1;
}
///////////////////////////////////////////////////////////////
// sets orthographic projection. screen coordinates are equal to world coordinates
void setOrtho(int wnd)
{
	glutSetWindow(wnd);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width/2,width/2,height/2,-height/2,-100,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
////////////////////////////////////////////////////////////////
void drawGrid(const Geom::AABB &bounds,int width,int height)
{
	glBegin(GL_LINES);
	for(float x=bounds.min(0);x<=bounds.max(0);x+=bounds.size(0)/width)
	{
		glVertex3f(x,bounds.min(0),bounds.min(2));
		glVertex3f(x,bounds.max(0),bounds.min(2));
	}
	for(float y=bounds.min(1);y<=bounds.max(1);y+=bounds.size(0)/height)
	{
		glVertex3f(bounds.min(0),y,bounds.min(2));
		glVertex3f(bounds.max(0),y,bounds.min(2));
	}
	glEnd();
}
////////////////////////////////////////////////////////////////
void drawArrow(const vec2f &start,const vec2f &end,float arrowWidth=0.1,float arrowLength=0.1)
{
 	vec2f dir=end-start;
	vec2f left(-dir[1],dir[0]);
	glBegin(GL_LINES);	
	glVertex2fv(end);
	glVertex2fv(start);

	glVertex2fv(end);
	glVertex2fv(end-left*arrowWidth*0.5-dir*arrowLength);

	glVertex2fv(end);
	glVertex2fv(end+left*arrowWidth*0.5-dir*arrowLength);
	glEnd();
}
/////////////////////////////////////////////////////////////////
void drawPoint(const vec3 &point,float size,int type)
{
	size*=0.5;
	glBegin(GL_LINES);
	if(type==0)	// cross
	{		
		glVertex3fv(point-vec3f(size,size,0));
		glVertex3fv(point+vec3f(size,size,0));

		glVertex3fv(point-vec3f(size,-size,0));
		glVertex3fv(point+vec3f(size,-size,0));
	}
	else if(type==1)	// box
	{
		glVertex3fv(point+vec3f(-size,-size,0));
		glVertex3fv(point+vec3f(-size, size,0));

		glVertex3fv(point+vec3f(-size, size,0));
		glVertex3fv(point+vec3f( size, size,0));

		glVertex3fv(point+vec3f( size, size,0));
		glVertex3fv(point+vec3f( size,-size,0));

		glVertex3fv(point+vec3f( size,-size,0));
		glVertex3fv(point+vec3f(-size,-size,0));
	}
	else if(type==2)	// rotated box
	{
		glVertex3fv(point+vec3f(-size,	 0,0));
		glVertex3fv(point+vec3f(	0,size,0));

		glVertex3fv(point+vec3f(	0,size,0));
		glVertex3fv(point+vec3f( size,0,0));

		glVertex3fv(point+vec3f( size,	 0,0));
		glVertex3fv(point+vec3f(	0,-size,0));

		glVertex3fv(point+vec3f(	0,-size,0));
		glVertex3fv(point+vec3f(-size,0,0));
	}
	glEnd();
}

void glTransform(const Pose2 & pose)
{
	glTranslatef(pose.position[0], pose.position[1], 0 );
	glRotatef( pose.orientation * 180.f / M_PI, 0, 0, 1 );
}

void glTransform(const Pose2z & pose)
{
	glTranslatef(pose.position[0], pose.position[1], pose.position[2] );
	glRotatef( pose.orientation * 180.f / M_PI, 0, 0, 1 );
}