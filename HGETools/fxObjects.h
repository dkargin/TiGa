#pragma once

#define	COLORADD    1
#define	COLORMUL    0

#define	ALPHABLEND  2
#define	ALPHAADD    0

#define	ZWRITE      4
#define	NOZWRITE    0

typedef float Time_t;

class FxManager;

class hgeSprite;
class hgeResourceManager;
class HGE;

struct FxView2
{
	Pose2z pose;
	float scale, zNear, zFar;
	hgeRect viewport;
};

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

class ObjectTracked
{
public:
	ObjectTracker::ID id;
	ObjectTracked();
	virtual ~ObjectTracked();
	static ObjectTracker effectTracker;
};

enum AnimationMode
{
	AnimationNoChange,		// used in FxEffect::start to preserve last animation mode
	AnimationHold,
	AnimationOnce,			// play all animations once
	AnimationRepeat,		// repeat every child animation
	AnimationRepeatGroup,	// repeat all animations
	AnimationBounce,
};

template<class Type>
class TreeNode
{
protected:
	typedef TreeNode<Type> node_type;
	Type * parent;
	Type * next, *prev, *head, *tail;
public:
	TreeNode()
	{
		parent = NULL;
		next = NULL;
		prev = NULL;
		head = NULL;
		tail = NULL;
	}
	virtual Type * getTargetType() = 0;
	void orphan_me()
	{
		if( parent != NULL )
			parent->removeChild(this);
	}
	bool hasChild(const node_type * child) const
	{
		return child && child->parent == this;
	}
	void attach(Type * newChild)
	{
		newChild->orphan_me();
		if( head == NULL)
		{
			head = newChild;
			tail = newChild;
			newChild->prev = NULL;
			newChild->next = NULL;
		}
		else
		{
			tail->next = newChild;
			newChild->prev = tail;
			newChild->next = NULL;
			tail = newChild;
		}
		newChild->parent = static_cast<Type*>(this);
	}
	void removeChild(node_type * child)
	{
		assert( hasChild(child) );
	}
	virtual void detach(node_type * child)
	{
		removeChild(child);
	}
	virtual ~TreeNode()
	{
		orphan_me();
	}
	class const_iterator
	{
		protected:
		const Type * container;
		const Type * current;
	public:
		typedef const_iterator iterator_type;
		const_iterator(const Type * container_, const Type * current_)
		{
			container = container_;
			current = current_;
		}
		const_iterator(const const_iterator &it)
		{
			container = it.container;
			current = it.current;
		}
		bool operator == ( const iterator_type & it) const
		{
			return container == it.container && current == it.current;
		}
		bool operator != ( const iterator_type & it) const
		{
			return container != it.container || current != it.current;
		}
		const Type * operator->()
		{
			return current;
		}
		iterator_type & operator++()	// prefix
		{
			assert(current != NULL);
			current = current->next;
			return *this;
		}
		iterator_type operator++(int)	// postfix
		{
			assert(current != NULL);
			const_iterator result(*this);
			current = current->next;
			return result;
		}
	};
	class iterator
	{
	protected:
		Type * container;
		Type * current;
	public:
		typedef iterator iterator_type;
		iterator(Type * container_, Type * current_)
		{
			container = container_;
			current = current_;
		}
		iterator(const const_iterator &it)
		{
			container = it.container;
			current = it.current;
		}
		bool operator == ( const iterator_type & it) const
		{
			return container == it.container && current == it.current;
		}
		bool operator != ( const iterator_type & it) const
		{
			return container == it.container && current == it.current;
		}
		Type * operator->()
		{
			return current;
		}
		iterator_type & operator++()	// prefix
		{
			assert(current != NULL);
			current = current->next;
			return *this;
		}
		iterator_type operator++(int)	// postfix
		{
			assert(current != NULL);
			const_iterator result(*this);
			current = current->next;
			return result;
		}
	};

	const_iterator begin() const
	{
		return const_iterator(static_cast<const Type*>(this), head);
	}
	const_iterator end() const
	{
		return const_iterator(static_cast<const Type*>(this), NULL);
	}
	iterator begin()
	{
		return iterator(static_cast<Type*>(this),head);
	}
	iterator end()
	{
		return iterator(static_cast<Type*>(this), NULL);
	}
};

class FxEffect : public Referenced, public TreeNode<FxEffect>
{
public:
	typedef SharedPtr<FxEffect> Pointer;
	typedef Pose2z Pose;
public:		
	enum FxType
	{
		fxHolder,
		fxSprite,
		fxAnimation,
		fxAnimation2,
		fxParticles,
		fxLight,
		fxBeam,
		fxSound,
		fxModel,
	};

	FxEffect(FxManager * manager);
	FxEffect(const FxEffect &effect);
	virtual ~FxEffect();

	FxEffect * getTargetType()
	{
		return this;
	}
	FxManager * getManager() const;
	Log * logger() const;
	// animation interface
	virtual void start(AnimationMode mode = AnimationNoChange);					// starts animation (if there is any)
	virtual void stop(bool immediate);		// stops animation (if there is any)
	virtual void rewind();					// rewind track
	virtual Time_t duration() const;			// get animation duration
	// geometry interface
	virtual void setPose(float x,float y,float r);
	virtual void setScale(float s);
	virtual float getScale() const;	
	virtual void setPose(const Pose::pos &v,const Pose::rot &r);
	virtual void setPose(const Pose &p);
	virtual const Pose & getPose() const;			// get pose relative to parent object
	Pose & poseRef() { return pose;}
	virtual Pose getGlobalPose()const;		

	float getWidth() const;				// get screen width
	float getHeight() const;			// get screen width	
	hgeRect getClipRect() const;		// return rect to determine, should we clip 

	virtual hgeRect getLocalRect() const;		// get bound only for root, excluding children
	// scene graph interface
	inline void show(bool flag) { visible = flag; }
	inline bool isVisible() const {return visible; }	
	virtual void query(FxManager * manager, const Pose & base);
	void queryAll(FxManager * manager, const Pose & base);	// query all hierarchy	
	virtual void render(FxManager * manager, const Pose & base);
	void renderAll(FxManager * manager, const Pose & base);					// render all hierarchy

	virtual void update(float dt);
	void updateAll( float dt );								// update all hierarchy
	
	virtual bool shouldClip(const FxView2 & view) const;	
	// misc
	virtual FxType type() const;
	virtual bool valid() const;
	virtual Pointer clone() const;
	HGE * hge() const;
protected:	
	virtual void onAttach( FxEffect * object )
	{
		Referenced::ObjectInfo::addReference(object);
	}
	virtual void onDetach( FxEffect * object )
	{
		Referenced::ObjectInfo::removeReference(object);
	}

	std::weak_ptr<FxManager> manager;	
	// geometry
	Pose pose;
	float scale;
	bool visible;
};

typedef FxEffect::Pointer FxPointer;

template<class FxType> inline FxType * CopyFactoryObject( std::weak_ptr<FxManager> manager, const FxType * source);

#define FX_TYPE(TargetType,TypeID) \
	friend class FxManager; \
	typedef SharedPtr<TargetType> Pointer; \
	typedef FxEffect Base; \
	Pointer copy() const \
	{\
		return CopyFactoryObject(manager, this); \
	}\
	FxEffect::Pointer clone() const \
	{ \
		return copy().get(); \
	} \
	FxType type()const \
	{ \
		return TypeID; \
	}

class FxSprite: public FxEffect
{
protected:
	FxSprite();
	FxSprite(FxManager *manager,HTEXTURE tex, float x, float y, float w, float h);
public:
	FX_TYPE(FxSprite,FxEffect::fxSprite);
	hgeSprite sprite;
	float scale_h;	
	FxSprite(hgeResourceManager &manager,const char *name);
	FxSprite(FxManager * manager);
	FxSprite(const FxSprite &spr);
	~FxSprite();
	void setBlendMode(int mode);
	void addBlendMode(int mode);
	void flipHor();
	void flipVer();
	virtual hgeRect getLocalRect() const;
	virtual bool valid() const;
	virtual void update(float dt);
	virtual void render(FxManager * manager, const Pose &base);	
};

typedef FxSprite::Pointer FxSpritePtr;

class FxSound: public FxEffect
{
public:
	HEFFECT sound;
	std::string name;
	bool positional;	/// if we affect loudness depending on viewer position
	HCHANNEL channel;
public:
	FX_TYPE(FxSound,FxEffect::fxSound);
	FxSound(FxManager * manager);
	//FxSound(FxManager * manager, HGE *hge, const char *file);
	FxSound(const FxSound &effect);
	~FxSound();
	void init( const char * name );
	virtual void stop(bool immediate);
	virtual void start(AnimationMode mode = AnimationNoChange);
	virtual Time_t duration() const;
	virtual void update(float dt);
	virtual void render(const Pose &base);
	virtual bool valid()const;
};

//class FxAnimation: public FxEffect//FxHelper<FxAnimation,FxEffect::fxAnimation>
//{
//public:
//	hgeAnimation animation;
//	FX_TYPE(FxAnimation,FxEffect::fxAnimation);
//	FxAnimation(FxManager * manager,HTEXTURE tex, int nframes, float FPS, float x, float y, float w, float h);
//	//FxAnimation(hgeResourceManager &manager,const char *name);
//	FxAnimation(const FxAnimation &effect);
//	~FxAnimation();
//	void start(AnimationMode mode = AnimationNoChange);
//	void stop(bool immediate);
//	virtual bool valid() const;
//	virtual void update(float dt);
//	virtual void render(const Pose &base);
//};

class FxParticles: public FxEffect
{
public:	
	struct Particle
	{
		vec2f position;
		vec2f velocity;
		float angle;			// model angle
		FxParticles * master;
	};
	hgeParticleSystem particles; 
	FX_TYPE(FxParticles,FxEffect::fxParticles);
	FxParticles(FxManager * manager);	
	FxParticles(const FxParticles &p);
	void init(hgeParticleSystemInfo &info);
	virtual bool valid() const;
	virtual void update(float dt);
	virtual void render(const Pose &base);
	/// animation interface
	virtual void start(AnimationMode mode = AnimationNoChange);
	virtual void stop(bool immediate);
	virtual Time_t duration() const;
};
typedef FxEffect FxHolder;

// All animation frames should be inside single texture page
class FxAnimation2: public FxEffect
{
	friend class FxManager;	
public:	
	FX_TYPE(FxAnimation2,FxEffect::fxAnimation2);
	FxAnimation2(FxManager * manager);
	FxAnimation2(FxManager *manager,HTEXTURE tex, float x, float y, float w, float h);
	FxAnimation2(const FxAnimation2 &effect);
	~FxAnimation2();
	void init(HTEXTURE tex, float x, float y, float w, float h);
	int addFrame(const fRect &rect);	// insert additional frame from existing texture
	void setBlendMode(int mode);
	void addBlendMode(int mode);
	bool valid() const;
	void update(float dt);
	/// animation interface
	void start( AnimationMode mode = AnimationNoChange );
	void stop(bool immediate);
	void rewind();
	Time_t duration() const;

	void render(FxManager * manager, const Pose &base);
	void setSize(float w,float h,bool mid=true);	// set tile size
	void xTile(float length);						// enable tiled mode
	bool isTiled()const;
	
	float cropWidth,cropHeight;	
	bool crop;
	bool drawRect;		// draw bounding rect
	float getWidth();	// returns sprite width
	float getHeight();	// returns sprite height
protected:	
	hgeSprite sprite;
	float width,height;				// world sprite size
	float tiledWidth, tiledHeight;	//
	bool tiled;						// if tiled mode is active. Turned off by default
	std::vector<fRect> frames;		// all frames	
	float fps;						// frame rate
	float current;					// current frame
	bool run;						// is animation active
	AnimationMode mode;
};

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

/////////////////////////////////////////////////////////////////////////
// Render queue with z&state-sorting
/////////////////////////////////////////////////////////////////////////
class RenderQueue
{
public:
	typedef FxEffect::Pose Pose;
	struct HeapEntry
	{
		Pose pose;
		FxEffect * effect;
	};
	std::vector<HeapEntry> objects;	
	std::vector<int> heap;
	void query(const Pose & base,FxEffect * effect);	
	void flush();			/// clear heap
	void render(FxManager * manager, const Pose & base);			/// render all stored effects
};
////////////////////////////////////////////////////////////////////////
// Storage for timed effects
////////////////////////////////////////////////////////////////////////
class Pyro
{
	struct TimedEffect
	{
		float left;
		FxPointer effect;
	};
	typedef FxEffect::Pose Pose;
	std::vector<TimedEffect> effects;
public:
	void runEffect(FxPointer effect);
	void update(FxManager * manager, float dt);
	void render(FxManager * manager, const Pose &pose,float scale);
};

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
/////////////////////////////////////////////////////////////////////////
// Resource manager and Fx factory
/////////////////////////////////////////////////////////////////////////


class FxManager : public std::enable_shared_from_this<FxManager>
{
public:
	//hgeResourceManager manager;
	HGE *hge;
	struct Record									/// Record for each resource file
	{
		std::string file;							/// file name
		DWORD handle;								/// assigned handle
		int ref;									/// reference counter
	};	

	//typedef std::list<FxPointer> Manufactured;
	//Manufactured manufactured;				/// keep all created objects here
	typedef std::list<Record> Records;				/// container type for all allocated resources
	Records textures,sounds;						/// Allocated resources
	RenderQueue renderQueue;	
	Pyro pyro;										/// holded effects will be destroyed when counter expires
	LogPtr log;
	FxManager(LogPtr logger);	//
	~FxManager();			
	void init(HGE * hge);			/// init resource system
	Log * logger() { return log; }
	/// factory
	FxAnimation2::Pointer createAnimation(const char *texture,fRect rect,int frameWidth,int frameHeight,float fps,AnimationMode mode);	// create animation from texture's part
	FxAnimation2::Pointer createAnimationFull(const char *texture,int frameWidth,int frameHeight,float fps,AnimationMode mode);			// create animation from whole texture
	FxParticles::Pointer fxParticles(FxSprite::Pointer sprite,hgeParticleSystemInfo &info);
	FxSprite::Pointer fxSprite(const char * texture,float x,float y,float width,float height);
	FxSprite::Pointer fxSolidQuad( float width, float height, DWORD color );
//	FxAnimation::Pointer fxAnimation(const char * texture,int frames,float fps);
	FxSound::Pointer fxSound(const char * path);
	FxHolder::Pointer fxHolder();
	//FxAnimation * fxAnimation(const char *name);
	//FxSprite * fxSprite(const char *name);
	//FxBeam::Pointer fxBeam(FxAnimation2 *start=NULL,FxAnimation2 *middle=NULL,FxAnimation2 *end=NULL);	
	void setView( const FxView2 & view );
	void resetView();

	void clearObjects();												// release manufactured list
	void clearResources();												// release all resources

	Record & getTexture(const char *texture);							//
	int freeTexture(HTEXTURE texture);									//

	
	bool allocateRaw(FxEffect *& effect);
	bool allocateRaw(FxSound *& effect);
	bool allocateRaw(FxSprite *& effect);
	bool allocateRaw(FxAnimation2 *& effect);
	bool allocateRaw(FxParticles *& effect);

	template< class FxType> FxType * create()
	{
		FxType * result = NULL;
		allocateRaw(result);
		new( result ) FxType(this);
		return result;
	}

	typedef std::shared_ptr<FxManager> SharedPtr;
	typedef std::weak_ptr<FxManager> WeakPtr;
protected:
	//FxEffect * remember(FxEffect *effect);
	Records::iterator find(const char *file);							//
	Records::iterator find(HTEXTURE tex);								//
	Pose2z viewPose;
	float viewScale;
};

typedef FxManager::SharedPtr FxManagerPtr;
#pragma push_macro("new")
#undef new
void FAR* operator new(size_t cb);
template<class FxType> inline FxType * CopyFactoryObject( std::weak_ptr<FxManager> manager, const FxType *source)
{
	FxType * result = NULL; 
	if(!source->valid())
		throw(std::exception("FxHelper::clone() error: invalid object\n"));
	else {
		FxType * place = NULL;		
		manager.lock()->allocateRaw(place);	
		result = new (place) FxType(*source);
	}
	return result;
}
#pragma pop_macro("new")

