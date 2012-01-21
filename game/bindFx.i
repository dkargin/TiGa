%{
#include "gameObject.h"
#include "fxObjects.h"

inline fRect Rect(int x,int y,int w,int h)
{
	return fRect(x,y,w,h);
}
%}

enum AnimationMode
{
	AnimationHold,
	AnimationOnce,			// play all animations once
	AnimationRepeat,		// repeat every child animation
	AnimationRepeatGroup,	// repeat all animations
	AnimationBounce,
};

#define	COLORADD    1
#define	COLORMUL    0
 
#define	ALPHABLEND  2
#define	ALPHAADD    0

#define	ZWRITE      4
#define	NOZWRITE    0

struct fRect
{
	float x,y,w,h;
	fRect();
	fRect(float x,float y,float w,float h);
	fRect(const fRect &rc);
	~fRect();
	bool	TestPoint(float px, float py) const;
};
fRect Rect(int x,int y,int w,int h);

class FxEffect
{
public:	
	enum FxType
	{
		fxHolder,
		fxSprite,
		fxAnimation,
		fxParticles,
		fxLight,
		fxSound,
	};
	void setScale(float s);
	virtual ~FxEffect(){};
	virtual void setPose(const Pose &p);
	virtual void setPose(vec2f pos,float r);
	virtual void setPose(float x,float y,float r);
	virtual Pose getPose() const;
	virtual FxType type()const=0;
	virtual bool valid()const=0;
	virtual void render(const Pose & base)=0;
	virtual void update(float dt)=0;
	virtual FxEffect * clone() const=0;
	virtual void query(const Pose & base);

	virtual void start(AnimationMode mode);			// starts animation (if there is any)
	virtual void stop(bool immediate);				// stops animation (if there is any)
	virtual void rewind();							// rewind track
};

%extend FxEffect
{
	Pose pose;	
	float scale;
	float z;
	bool visible;

	void setPose3(const vec3f & v, float r)
	{
		$self->setPose(Pose(v,r));
	}
	SharedPtr<FxEffect> base() 
	{
		return $self;
	}
}
%{

float FxEffect_z_get(FxEffect * v)
{
	return v->poseRef().position[2];
}

void FxEffect_z_set(FxEffect * v, float value)
{
	v->poseRef().position[2] = value;
}

Pose * FxEffect_pose_get(FxEffect *v) 
{
	return new Pose(v->getPose());
}

void FxEffect_pose_set(FxEffect *v, Pose *pose)
{
	v->setPose(*pose);
}

float FxEffect_scale_get(FxEffect *v)
{
	return v->getScale();
}

void FxEffect_scale_set(FxEffect *v, float scale)
{
	v->setScale(scale);
}

bool FxEffect_visible_get(FxEffect *v)
{
	return v->isVisible();
}

void FxEffect_visible_set(FxEffect *v,bool visible)
{
	v->show(visible);
}
%}

%template(FxEffectPtr) SharedPtr<FxEffect>;
typedef SharedPtr<FxEffect> FxEffectPtr;

class FxSprite: public FxEffect
{
public:
	~FxSprite();
	void setBlendMode(int mode);
	void addBlendMode(int mode);	
};

%template(FxSpritePtr) SharedPtr<FxSprite>;
typedef SharedPtr<FxSprite> FxSpritePtr;


class FxParticles: public FxEffect
{};

class FxAnimation2: public FxEffect
{	
public:
	bool crop;
	float cropWidth,cropHeight;
	int addFrame(const fRect &rect);
	float duration()const;
	void setBlendMode(int mode);
	void addBlendMode(int mode);
	int getWidth();
	int getHeight();
	void setSize(float w,float h,bool mid);
	void start(AnimationMode mode);
	void stop(bool immediate);
};

class FxSound: public FxEffect
{
public:
	bool positional;
};

class FxManager
{
public:
	FxAnimation2 * createAnimation(const char *texture,fRect rect,int frameWidth,int frameHeight,float fps,AnimationMode mode);
	FxAnimation2 * createAnimationFull(const char *texture,int frameWidth,int frameHeight,float fps,AnimationMode mode);
	FxSpritePtr fxSprite(const char * texture,float x,float y,float width,float height);
	FxParticles * fxParticles(FxSprite* sprite, hgeParticleSystemInfo &info);
	FxSound * fxSound(const char * source);
	FxHolder * fxHolder();
};