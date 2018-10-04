
#include <algorithm>
#include <GL/gl.h>

#include "fxmanager.h"
#include "fxobjects.h"
#include "fxsound.h"

namespace Fx
{

FxManager::FxManager()
{
	//storage = std::shared_ptr<Storage>(new HeapAllocator);

	// Shader sources
	const GLchar* vertexSource = R"(
	attribute vec4 position;
	void main()
	{
		 gl_Position = vec4(position.xyz, 1.0);
	})";

	const GLchar* fragmentSource = R"(
	precision mediump float;
	void main()
	{
	  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0 );
	})";

}

FxManager::~FxManager()
{
	clearObjects();
	clearResources();
	textureManager.reset(nullptr);
}

void FxManager::init()
{
	textureManager.reset(new TextureManager());
}

TextureManager* FxManager::getTextureManager()
{
	return textureManager.get();
}

void FxManager::clearObjects()
{
}

void FxManager::clearResources()
{
	for(Records::iterator it = textures.begin();it != textures.end(); ++it)
		textureManager->free(it->handle);
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
		
		if(!(record.handle = textureManager->loadFile(texture)))
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
	{
		if(--(it->ref))
		{
			textureManager->free(it->handle);
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
	FxSprite * result = new FxSprite(this);
	result->sprite.setTextureRect(0, 0, width, height);
	result->sprite.setColor( color );
	return FxSpritePtr(result);
}

FxSpritePtr FxManager::fxSprite(const char * texture,float x,float y,float width,float height)
{
	FxTextureId tex = getTexture(texture).handle;
	if(!tex)
		return NULL;
	if(width == 0 && height == 0 && x == 0 && y == 0)
	{
		width = textureManager->width(tex);
		height= textureManager->height(tex);
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
	result->sprite.setBlendMode(BLEND_COLORMUL|BLEND_ALPHAADD);
	FxTextureId tex=result->sprite.getTexture();
	int texWidth = textureManager->width(tex);
	int texHeight = textureManager->height(tex);
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
	result->sprite.setBlendMode(BLEND_COLORMUL|BLEND_ALPHAADD);
	FxTextureId tex=result->sprite.getTexture();
	// 1. compute frames
	int texWidth = textureManager->width(tex);
	int texHeight = textureManager->height(tex);
	if(frameWidth == 0 && frameHeight == 0)
	{
		frameWidth = textureManager->width(tex);
		frameHeight = textureManager->height(tex);
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

} // namespace Fx
