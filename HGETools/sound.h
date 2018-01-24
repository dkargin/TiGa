#pragma once

#include "fxObjects.h"

namespace Fx
{

typedef int32_t HEFFECT;
typedef int32_t HCHANNEL;
/**
 * FxSound class
 * Encapsulates positional sound effect
 */
class FxSound: public Entity
{
public:
	HEFFECT sound;
	std::string name;
	bool positional;	/// if we affect loudness depending on viewer position
	HCHANNEL channel;
public:
	FX_TYPE(FxSound, EffectType::fxSound);
	FxSound();
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

}
