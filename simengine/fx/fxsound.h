#pragma once

#include "fxobjects.h"
#include "soundmanager.h"

namespace Fx
{

class SoundManager;


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
	FxSound(SoundManager* sm);
	~FxSound();

	void init( const char * name );
	virtual void stop(bool immediate) override;
	virtual void start(AnimationMode mode = AnimationNoChange) override;
	virtual Time_t duration() const override;
	virtual void update(float dt) override;
	virtual void render(RenderContext* context, const Pose& base) override;
	virtual bool valid()const override;

protected:
	FxSound(const FxSound &effect);

	SoundManager* sm;
};

}
