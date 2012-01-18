#include "stdafx.h"
#include <algorithm>
#include "fxObjects.h"
#include "gl/gl.h"

//////////////////////////////////////////////////////////////////////////
// FxManager
// implements resource managment and lighting
//////////////////////////////////////////////////////////////////////////
FxManager::FxManager(Log * log)
	:hge(NULL), log(log)
{
	resetView();
}

FxManager::~FxManager()
{
	LogFunction(*log);
	clearObjects();
	clearResources();	
}

void FxManager::init( HGE * hge )
{
	this->hge = hge;
}

void FxManager::clearObjects()
{
	LogFunction(*log);
//	manufactured.clear();
}

void FxManager::clearResources()
{
	LogFunction(*log);
//	if(!manufactured.empty())
//	{
//		log->line(2,"there are still some fxObjects");
//		//_CrtDbgBreak();
//	}
	for(Records::iterator it = textures.begin();it != textures.end(); ++it)
		hge->Texture_Free(it->handle);
	textures.clear();
}

FxManager::Records::iterator FxManager::find(const char *file)
{
	Records::iterator it = textures.begin();
	for(;it != textures.end(); ++it)
		if(it->file == file)
			break;
	return it;
}

FxManager::Records::iterator FxManager::find(HTEXTURE tex)
{
	Records::iterator it = textures.begin();
	for(;it != textures.end(); ++it)
		if(it->handle == tex)
			break;
	return it;
}

FxManager::Record & FxManager::getTexture(const char *texture)
{
	LogFunction(*log);
	auto it = find(texture);
	if(it == textures.end())	// load new texture
	{
		Record record;
		record.file = texture;
		
		if(!(record.handle = hge->Texture_Load(texture)))
		{
			log->line(2,"Cannot load texture <%s>",texture);
			throw(std::exception("FxManager::getTexture(): cannot load texture"));
		}
		record.ref = 0;
		textures.push_back(record);
		it = --textures.end();
	}
	it->ref++;
	return *it;
}

int FxManager::freeTexture(HTEXTURE texture)
{
	LogFunction(*log);
	auto it = find(texture);
	if(it != textures.end())
	{
		if(--(it->ref))
		{
			hge->Texture_Free(it->handle);
			textures.erase(it);
		}
		else
			return it->ref;
	}
	return 0;
}

FxSound::Pointer FxManager::fxSound(const char * path)
{
	FxSound * result = create<FxSound>();
	result->init( path);
	return result;
}

FxSprite::Pointer FxManager::fxSolidQuad( float width, float height, DWORD color )
{
	LogFunction(*log);
	FxSprite * result = create<FxSprite>();
	result->sprite.SetTextureRect(0, 0, width, height);
	result->sprite.SetColor( color );
	return result;
}

FxSprite::Pointer FxManager::fxSprite(const char * texture,float x,float y,float width,float height)
{
	LogFunction(*log);
	HTEXTURE tex = getTexture(texture).handle;
	if(!tex)
		return NULL;
	if(width == 0 && height == 0 && x == 0 && y == 0)
	{
		width = hge->Texture_GetWidth(tex);
		height= hge->Texture_GetHeight(tex);
	}
	FxSprite *result = new FxSprite(this, tex, x, y, width, height);
	return result;
}

FxParticles::Pointer FxManager::fxParticles(FxSprite::Pointer sprite,hgeParticleSystemInfo &info)
{
	FxParticles * result = NULL;
	/*
	// TODO: particles effect remake
	if(sprite != NULL)
	{
		hgeParticleSystemInfo psi = info;
		psi.sprite = &sprite->sprite;
		result = new FxParticles(this, psi);
	}*/
	return result;
}

FxHolder::Pointer FxManager::fxHolder()
{	
	return create<FxHolder>();
}

FxAnimation2::Pointer FxManager::createAnimation(const char *file, fRect rect, int frameWidth, int frameHeight, float fps, AnimationMode mode)
{
	LogFunction(*log);
	if( hge == NULL )
		return NULL;
	// 0. create object
	FxAnimation2 * result=new FxAnimation2(this,getTexture(file).handle,0,0,64,64);
	result->fps=fps;
	result->mode=mode;	
	result->sprite.SetBlendMode(BLEND_COLORMUL|BLEND_ALPHAADD);
	HTEXTURE tex=result->sprite.GetTexture();
	int texWidth=hge->Texture_GetWidth(tex);
	int texHeight=hge->Texture_GetHeight(tex);
	// 1. compute frames
	if(rect.w == 0 && rect.h == 0)
	{
		rect.w = texWidth;
		rect.h = texHeight;
	}
	if(frameWidth == 0 && frameHeight == 0)
	{
		frameWidth = rect.w;
		frameHeight = rect.h;
	}
	int framesX=rect.w/frameWidth;
	int framesY=rect.h/frameHeight;
	for(int y=0;y<framesY;y++)
		for(int x=0;x<framesX;x++)
		{
			fRect frame;
			frame.x=rect.x+x*frameWidth;
			frame.y=rect.y+y*frameHeight;
			frame.w=frameWidth;
			frame.h=frameHeight;
			result->addFrame(frame);
		}
	result->setSize(frameWidth,frameHeight);
//	remember(result);
	return result;
}

FxAnimation2::Pointer FxManager::createAnimationFull(const char *file, int frameWidth, int frameHeight, float fps, AnimationMode mode)
{
	if( hge == NULL )
		return NULL;
	LogFunction(*log);
	// 0. create object
	FxAnimation2 * result = new FxAnimation2(this,getTexture(file).handle,0,0,64,64);
	result->fps=fps;
	result->mode=mode;	
	result->sprite.SetBlendMode(BLEND_COLORMUL|BLEND_ALPHAADD);
	HTEXTURE tex=result->sprite.GetTexture();
	// 1. compute frames
	int texWidth = hge->Texture_GetWidth(tex);
	int texHeight = hge->Texture_GetHeight(tex);
	if(frameWidth == 0 && frameHeight == 0)
	{
		frameWidth = hge->Texture_GetWidth(tex);
		frameHeight = hge->Texture_GetHeight(tex);
	}
	int framesX = frameWidth?texWidth/frameWidth:frameWidth;
	int framesY = frameHeight?texHeight/frameHeight:frameHeight;
	for(int y=0;y<framesY;y++)
		for(int x=0;x<framesX;x++)
		{
			fRect frame;
			frame.x = x*frameWidth;
			frame.y = y*frameHeight;
			frame.w = frameWidth;
			frame.h = frameHeight;
			result->addFrame(frame);
		}

	result->setSize(frameWidth,frameHeight);
//	remember(result);
	return result;
}

void FxManager::setView( const FxView2 & view )
{
	if( hge == NULL )
		return;
	viewPose = view.pose;
	viewScale = view.scale;
	int screenWidth = hge->System_GetState(HGE_SCREENWIDTH);
	int screenHeight = hge->System_GetState(HGE_SCREENHEIGHT);
	float x = -viewPose.position[0];
	float y = -viewPose.position[1];
	float z = -viewPose.position[2];

	// TODO: PORT: replace direct OGL calls to HGE calls
	float angle = viewPose.orientation * 180/M_PI;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-screenWidth/2,screenWidth/2,screenHeight/2,-screenHeight/2,-1,1);	
	glScalef(viewScale, viewScale,1);
	glRotatef(-angle,0,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(x,y,z);
}

void FxManager::resetView()
{
	viewPose = Pose2z(0,0,0,0);
	viewScale = 1.0;
}

///////////////////////////////////////////////////////////////////////////////////////
// Object allocateRawion
template<class Type> void allocRaw( Type *& raw )
{
	raw = (Type * ) new char[sizeof (Type) ];
}

bool FxManager::allocateRaw(FxEffect *& effect) { allocRaw(effect); return effect != NULL; }
bool FxManager::allocateRaw(FxSound *& effect) { allocRaw(effect); return effect != NULL;}
bool FxManager::allocateRaw(FxSprite *& effect) { allocRaw(effect); return effect != NULL;}
bool FxManager::allocateRaw(FxAnimation2 *& effect) { allocRaw(effect); return effect != NULL;}
bool FxManager::allocateRaw(FxParticles *& effect) { allocRaw(effect); return effect != NULL;}