#pragma once

#include <treenode.h>
#include <set>
#include <memory> 		//for shared_ptr

#include "basetypes.h"
#include "spritedata.h"

//#include "hge.h"

#define	COLORADD    1
#define	COLORMUL    0

#define	ALPHABLEND  2
#define	ALPHAADD    0

#define	ZWRITE      4
#define	NOZWRITE    0

class hgeResourceManager;
class HGE;

namespace Fx
{

// Helper classes
class ParticleSystemInfo;


// Fs object classes
class Entity;
class FxSound;
class FxSprite;
class FxAnimation2;
class FxParticles;

class FxManager;
class RenderContext;

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::shared_ptr<FxSound> FxSoundPtr;
typedef std::shared_ptr<FxSprite> FxSpritePtr;
typedef std::shared_ptr<FxParticles> FxParticlesPtr;
typedef std::shared_ptr<FxAnimation2> FxAnimation2Ptr;


// Contains view pose + scale + range limits
struct FxView2
{
	math3::Pose2z pose;
	float scale, zNear, zFar;
	Fx::Rect viewport;
};

// Get rid if this shit
struct ObjectTracker
{
	typedef unsigned int ID;
	ID lastID;
	std::set<ID> assigned;
	std::set<ID> breaks;

	ID allocID();
	void freeID(ID id);
	void check();
};

#ifdef FUCK_THIS
// Get rid if this shit
class ObjectTracked
{
public:
	ObjectTracker::ID id;
	ObjectTracked();
	virtual ~ObjectTracked();
	static ObjectTracker effectTracker;
};
#endif

#define FX_TYPE(TargetType,TypeID) \
	friend class FxManager; \
	typedef std::shared_ptr<TargetType> Pointer; \
	Entity * clone() const \
	{ \
		return new TargetType(*this); \
	} \
	EffectType type()const \
	{ \
		return TypeID; \
	}

enum AnimationMode
{
	AnimationNoChange,		// used in FxEffect::start to preserve last animation mode
	AnimationHold,
	AnimationOnce,			// play all animations once
	AnimationRepeat,		// repeat every child animation
	AnimationRepeatGroup,	// repeat all animations
	AnimationBounce,
};

enum class EffectType
{
	EffectGroup,
	fxSprite,
	fxAnimation,
	fxAnimation2,
	fxParticles,
	fxLight,
	fxBeam,
	fxSound,
	fxModel,HTEXTURE
};

/**
 * Base class for any effect in scene graph
 */
class Entity :
		public std::enable_shared_from_this<Entity>,
		public TreeNode<Entity>
{
public:
	Entity();

	virtual ~Entity();

	Entity * getTargetType()
	{
		return this;
	}

	virtual void start(AnimationMode mode = AnimationNoChange);					// starts animation (if there is any)
	virtual void stop(bool immediate);		// stops animation (if there is any)
	virtual void rewind();								// rewind track
	virtual Time_t duration() const;			// get animation duration
	// geometry interface
	void setZ(float z);
	float getZ() const;
	virtual void setPose(float x,float y,float r);
	virtual void setScale(float s);
	virtual float getScale() const;	
	virtual void setPose(const Pose::pos &v,const Pose::rot &r);
	virtual void setPose(const Pose &p);
	virtual const Pose & getPose() const;			// get pose relative to parent object
	Pose & poseRef() { return pose;}
	virtual Pose getGlobalPose()const;

	float getWidth() const;							//< Get screen width
	float getHeight() const;						//< Get screen width

	Rect getClipRect() const;						//< return rect to determine, should we clip
	virtual Rect getLocalRect() const;	//< get bound only for root, excluding children
	// scene graph interface
	void setVisible(bool flag);
	bool isVisible() const;

#ifdef FUCK_THIS_LESS
	virtual void query(FxManager * manager, const Pose& base);
	void queryAll(FxManager * manager, const Pose& base);			// query all hierarchy
#endif
	virtual void render(RenderContext* context, const Pose& base);
	void renderAll(RenderContext* manager, const Pose& base);					// render all hierarchy

	virtual void update(float dt);
	void updateAll( float dt );								//< update all hierarchy
	
	virtual bool shouldClip(const FxView2 & view) const;	
	// misc
	virtual EffectType type() const;
	virtual bool valid() const;
	virtual Entity * clone() const;
protected:
	Entity(const Entity &effect);

	virtual void onAttach( Entity * object );
	virtual void onDetach( Entity * object );

	Pose pose;
	float scale;
	bool visible;
};

/*
 * Render queue with z&state-sorting
 */
class RenderQueue
{
public:
	struct HeapEntry
	{
		Pose pose;
		EntityPtr effect;
	};

	std::vector<HeapEntry> objects;
	std::vector<int> heap;

	void enqueue(const Pose & base, EntityPtr effect);
	void flush();			/// clear heap
	void render(RenderContext* rc, const Pose & base);			/// render all stored effects
protected:

};


/**
 * Sprite in scene graph
 */
class FxSprite: public Entity
{
protected:
	FxSprite();
	FxSprite(FxManager *manager, FxTextureId tex, float x, float y, float w, float h);

public:
	FX_TYPE(FxSprite, EffectType::fxSprite);

	FxSprite(hgeResourceManager &manager,const char *name);
	FxSprite(FxManager * manager);

	FxSprite(const FxSprite &spr);
	~FxSprite();
	
	void setBlendMode(int mode);
	void addBlendMode(int mode);
	void flipHor();
	void flipVer();

	virtual Rect getLocalRect() const;
	virtual bool valid() const override;
	virtual void update(float dt) override;
	virtual void render(RenderContext* manager, const Pose &base) override;

protected:
	SpriteData sprite;
	float scale_h;	
};

/**
 * Animation class
 *
 * All animation frames should be inside single texture page
 */
//
class FxAnimation2: public Entity
{
	friend class FxManager;	
public:	
	FX_TYPE(FxAnimation2, EffectType::fxAnimation2);

	FxAnimation2(FxManager* manager);
	FxAnimation2(FxManager* manager,FxTextureId tex, float x, float y, float w, float h);
	FxAnimation2(const FxAnimation2& effect);
	~FxAnimation2();
	void init(FxTextureId tex, float x, float y, float w, float h);
	int addFrame(const Rect& rect);				// insert additional frame from existing texture
	void setBlendMode(int mode);
	void addBlendMode(int mode);
	bool valid() const;
	void update(float dt);
	/// animation interface
	void start( AnimationMode mode = AnimationNoChange );
	void stop(bool immediate);
	void rewind();
	Time_t duration() const;

	void render(FxManager* manager, const Pose &base);
	void setSize(float w,float h,bool mid=true);	// set tile size
	void xTile(float length);						// enable tiled mode
	bool isTiled()const;
	
	float cropWidth,cropHeight;	
	bool crop;
	bool drawRect;				//< draw bounding rect
	float getWidth();			//< returns sprite width
	float getHeight();		//< returns sprite height
protected:	
	SpriteData sprite;
	float width,height;						//< world sprite size
	float tiledWidth, tiledHeight;	//< Number of tiles to be rendered
	bool tiled;									//< if tiled mode is active. Turned off by default
	std::vector<Rect> frames;		//< all frames
	float fps;									//< frame rate
	float current;							//< current frame
	bool run;										//< is animation active
	AnimationMode mode;
};

}	// namespace Fx

//// Light source. Also may cast shadows
//class FxLight: public FxHelper<FxLight,FxEffect::fxLight>
//{
//public:
//	bool shadows;
//	vec4f color;	// light color
//	float size;		//
//	float angle;	//
//
//	FxLight(FxManager *_manager);
//	FxLight(const FxLight &light);
//	~FxLight();
//	virtual bool valid() const;
//	virtual void update(float dt);
//	virtual void render(const Pose &base);
//};

//class ResourceManager
//{
//public:
//	struct ResFile
//	{
//		std::string file;
//		int ref;
//		virtual void free(ResourceManager * manager) = 0;
//	};
//	struct ResTexture : public ResFile
//	{
//		DWORD handle;
//		virtual void free(ResourceManager * manager);
//	};
//	struct ResSound : public ResFile
//	{
//		DWORD handle;
//		virtual void free(ResourceManager * manager);
//	};
//};

// WTF Is this shit???
#ifdef FUCK_THIS
#pragma push_macro("new")
#undef new
void FAR* operator new(size_t cb);
template<class FxType> inline FxType * CopyFactoryObject( std::weak_ptr<FxManager> manager, const FxType *source)
{
	FxType * result = NULL; 
	if(!source->valid())
		throw(std::exception("FxHelper::clone() error: invalid object"));
	else {
		//FxType * place = NULL;		
		//manager.lock()->allocateRaw(place);	
		result = new FxType(*source);
	}
	return result;
}
#pragma pop_macro("new")
#endif //FUCK_THIS

