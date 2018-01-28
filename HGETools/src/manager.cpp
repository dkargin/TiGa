#include <fxmanager.h>
#include <algorithm>
#include <GL/gl.h>

#include "fxobjects.h"
#include "fxsound.h"




namespace Fx
{

class HeapAllocator : public FxManager::Storage
{
public:
	bool allocateRaw(Entity *& effect);
	bool allocateRaw(FxSound *& effect);
	bool allocateRaw(FxSprite *& effect);
	bool allocateRaw(FxAnimation2 *& effect);
	bool allocateRaw(FxParticles *& effect);
};


FxManager::FxManager()
{
	resetView();
	//storage = std::shared_ptr<Storage>(new HeapAllocator);
	textureManager = nullptr;
}

FxManager::~FxManager()
{
	clearObjects();
	clearResources();	
}

void FxManager::init( HGE * hge )
{
	//this->hge = hge;
}

void FxManager::clearObjects()
{
}

void FxManager::clearResources()
{
	for(Records::iterator it = textures.begin();it != textures.end(); ++it)
		textureManager->Texture_Free(it->handle);
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

FxManager::Records::iterator FxManager::find(FxTextureId tex)
{
	Records::iterator it = textures.begin();
	for(;it != textures.end(); ++it)
		if(it->handle == tex)
			break;
	return it;
}

FxManager::Record & FxManager::getTexture(const char *texture)
{
	auto it = find(texture);
	if(it == textures.end())	// load new texture
	{
		Record record;
		record.file = texture;
		record.handle = 0;
		
		if(!(record.handle = textureManager->Texture_Load(texture)))
		{
			//log->line(2,"Cannot load texture <%s>",texture);
			throw (std::runtime_error("FxManager::getTexture(): cannot load texture"));
		}
		record.ref = 0;
		textures.push_back(record);
		it = --textures.end();
	}
	it->ref++;
	return *it;
}

int FxManager::freeTexture(FxTextureId texture)
{
	auto it = find(texture);
	if(it != textures.end())
	{typedef int32_t HEFFECT;
		if(--(it->ref))
		{
			textureManager->Texture_Free(it->handle);
			textures.erase(it);
		}
		else
			return it->ref;
	}
	return 0;
}

FxSoundPtr FxManager::fxSound(const char * path)
{
	FxSound * result = new FxSound(nullptr);
	result->init( path);
	return FxSoundPtr(result);
}

FxSpritePtr FxManager::fxSolidQuad( float width, float height, FxRawColor color )
{
	FxSprite * result = new FxSprite();
	result->sprite.SetTextureRect(0, 0, width, height);
	result->sprite.SetColor( color );
	return FxSpritePtr(result);
}

FxSpritePtr FxManager::fxSprite(const char * texture,float x,float y,float width,float height)
{
	FxTextureId tex = getTexture(texture).handle;
	if(!tex)
		return NULL;
	if(width == 0 && height == 0 && x == 0 && y == 0)
	{
		width = textureManager->Texture_GetWidth(tex);
		height= textureManager->Texture_GetHeight(tex);
	}
	FxSprite *result = new FxSprite(this, tex, x, y, width, height);
	return FxSpritePtr(result);
}

#ifdef FUCK_THIS
FxParticlesPtr FxManager::fxParticles(FxSprite::Pointer sprite, ParticleSystemInfo &info)
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
	return FxParticlesPtr(result);
}
#endif

EntityPtr FxManager::fxHolder()
{	
	return EntityPtr(new Entity());
}

FxAnimation2Ptr FxManager::createAnimation(const char *file, Rect rect, int frameWidth, int frameHeight, float fps, AnimationMode mode)
{
	// 0. create object
	FxAnimation2 * result=new FxAnimation2(this,getTexture(file).handle,0,0,64,64);
	result->fps=fps;
	result->mode=mode;	
	result->sprite.SetBlendMode(BLEND_COLORMUL|BLEND_ALPHAADD);
	FxTextureId tex=result->sprite.GetTexture();
	int texWidth = textureManager->Texture_GetWidth(tex);
	int texHeight = textureManager->Texture_GetHeight(tex);
	// 1. compute frames
	if(rect.width() == 0 && rect.width() == 0)
	{
		rect.x2 = rect.x1 + texWidth;
		rect.y2 = rect.y1 + texHeight;
	}
	if(frameWidth == 0 && frameHeight == 0)
	{
		frameWidth = rect.width();
		frameHeight = rect.height();
	}
	int framesX = rect.width() / frameWidth;
	int framesY = rect.height() / frameHeight;
	for(int y=0;y<framesY;y++)
		for(int x=0;x<framesX;x++)
		{
			Rect frame;
			frame.x1 = rect.x1 + x*frameWidth;
			frame.y1 = rect.y1 + y*frameHeight;
			frame.x2 = frame.x1 + frameWidth;
			frame.y2 = frame.y1 + frameHeight;
			result->addFrame(frame);
		}
	result->setSize(frameWidth,frameHeight);
//	remember(result);
	return FxAnimation2Ptr(result);
}

FxAnimation2Ptr FxManager::createAnimationFull(const char *file, int frameWidth, int frameHeight, float fps, AnimationMode mode)
{
	// 0. create object
	FxAnimation2 * result = new FxAnimation2(this,getTexture(file).handle,0,0,64,64);
	result->fps=fps;
	result->mode=mode;	
	result->sprite.SetBlendMode(BLEND_COLORMUL|BLEND_ALPHAADD);
	FxTextureId tex=result->sprite.GetTexture();
	// 1. compute frames
	int texWidth = textureManager->Texture_GetWidth(tex);
	int texHeight = textureManager->Texture_GetHeight(tex);
	if(frameWidth == 0 && frameHeight == 0)
	{
		frameWidth = textureManager->Texture_GetWidth(tex);
		frameHeight = textureManager->Texture_GetHeight(tex);
	}
	int framesX = frameWidth?texWidth/frameWidth:frameWidth;
	int framesY = frameHeight?texHeight/frameHeight:frameHeight;
	for(int y=0;y<framesY;y++)
		for(int x=0;x<framesX;x++)
		{
			Rect frame;
			frame.x1 = x*frameWidth;
			frame.y1 = y*frameHeight;
			frame.x2 = frame.x1 + frameWidth;
			frame.y2 = frame.y1 + frameHeight;
			result->addFrame(frame);
		}

	result->setSize(frameWidth,frameHeight);
	return FxAnimation2Ptr(result);
}

void FxManager::setView( const FxView2 & view, int screenWidth, int screenHeight )
{
	viewPose = view.pose;
	viewScale = view.scale;
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
	viewPose = math3::Pose2z(0,0,0,0);
	viewScale = 1.0;
}

} // namespace Fx
