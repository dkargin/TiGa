#pragma once

#include "fxobjects.h"
#include "basetypes.h"

#include "texture_manager.h"

namespace Fx
{

class TextureManager;
/**
 * Storage for timed effects
 */
class Pyro
{
	struct TimedEffect
	{
		float left;
		EntityPtr effect;
	};

	std::vector<TimedEffect> effects;
public:
	void runEffect(EntityPtr effect);
	void update(FxManager * manager, float dt);
	void render(RenderQueue* queue, const Pose &pose,float scale);
};

/**
 * Resource manager and factory for any fx object
 */
class FxManager : public std::enable_shared_from_this<FxManager>
{
public:
	typedef void* HGE;
	struct Record								//< Record for each resource file
	{
		std::string file;					//< file name
		int32_t handle;						//< assigned handle
		int ref;									//< reference counter
	};

	typedef std::list<Record> Records;				/// container type for all allocated resources
	Records textures, sounds;		//< Storage of allocated resources
	RenderQueue renderQueue;
	Pyro pyro;									//< stored effects will be destroyed when counter expires

	FxManager();
	~FxManager();

	/// init resource system
	void init();

	// Create animation from the part of texture
	FxAnimation2Ptr createAnimation(const char *texture, Rect rect, int frameWidth, int frameHeight, float fps, AnimationMode mode);
	// Create animation from the whole texture
	FxAnimation2Ptr createAnimationFull(const char *texture, int frameWidth, int frameHeight, float fps, AnimationMode mode);
	FxParticlesPtr fxParticles(FxSpritePtr sprite, ParticleSystemInfo &info);
	FxSpritePtr fxSprite(const char * texture,float x,float y,float width,float height);
	FxSpritePtr fxSolidQuad( float width, float height, FxRawColor color );
	FxSoundPtr fxSound(const char * path);
	EntityPtr fxHolder();

	void setView( const FxView2& view, int screen_width, int screen_height);
	void resetView();

	void clearObjects();												// release manufactured list
	void clearResources();												// release all resources

	Record& getTexture(const char *texture);							//
	int freeTexture(FxTextureId texture);									//
	TextureManager* getTextureManager();

protected:
	Records::iterator find(const char *file);							//< Find resource by its filename
	Records::iterator find(FxTextureId tex);							//< Find resource by its texture id
	math3::Pose2z viewPose;
	float viewScale;
	std::unique_ptr<TextureManager> textureManager;
};

}
