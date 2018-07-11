#include "fxobjects.h"
#include "fxsound.h"

namespace Fx
{

FxSound::FxSound(SoundManager* sm)
{
	this->sm = sm;
	sound = 0;
	channel = 0;
	positional = true;
}

FxSound::FxSound(const FxSound &effect)
	:Entity(effect)
{
	sm = effect.sm;
	channel = 0;
	sound = 0;
	positional = effect.positional;
	init(effect.name.c_str());
}

FxSound::~FxSound()
{
	sm->free(sound);
}

void FxSound::init(const char * name)
{
	sound = sm->load(name);
	this->name = name;
}

void FxSound::start( AnimationMode mode )
{
	// Start all children
	Entity::start(mode);

	channel = sm->play(sound);
}

void FxSound::stop( bool immediate )
{
	// Stop all children
	Entity::stop(immediate);

	if(channel)
	{
		sm->stop(channel);
		channel = 0;
	}
}

Time_t FxSound::duration() const
{
	if(channel)
		return sm->getLength(channel);
	return 0;
}

void FxSound::update(float dt)
{
	// TODO: implement timeline
	// TODO: implement fading
	//float length = (getPose()).position.length();
	//const float distanceScale = 10;
	//float volume = 100 / 100 + distanceScale * length;
	//sm->Channel_SetVolume(channel, volume);
}

void FxSound::render(RenderContext* context, const Pose& base)
{
	
}

bool FxSound::valid()const
{
	return sound != 0;
}

} // namespace Fx
