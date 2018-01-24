#include "stdafx.h"
#include "fxObjects.h"

///////////////////////////////////////////////////////
// Sound
///////////////////////////////////////////////////////
FxSound::FxSound(FxManager * manager)
	:FxEffect(manager), sound(0), channel(0)
{
	//_RPT0(0,"FxSound::FxSound()\n");
}

FxSound::FxSound(const FxSound &effect)
	:FxEffect(effect)	
{
	//_RPT0(0,"FxSound::FxSound(const FxSound &spr)\n");
	init( effect.name.c_str() );
	
}

FxSound::~FxSound()
{
	hge()->Effect_Free(sound);
}

void FxSound::init( const char * name )
{
	sound = hge()->Effect_Load(name);
	this->name = name;
}

void FxSound::start( AnimationMode mode )
{
	FxEffect::start(mode);
	channel = hge()->Effect_Play(sound);	
}

void FxSound::stop( bool immediate )
{
	FxEffect::stop(immediate);
	if(channel)
	{
		hge()->Channel_Stop(channel);
		channel = 0;
	}
}

Time_t FxSound::duration() const
{
	if(channel)
		return hge()->Channel_GetLength(channel);
	return 0;
}

void FxSound::update(float dt)
{
	// TODO: implement timeline
	// TODO: implement fading
	//float length = (getPose()).position.length();
	//const float distanceScale = 10;
	//float volume = 100 / 100 + distanceScale * length;
	//hge->Channel_SetVolume(channel, volume);
}

void FxSound::render(const Pose &base)
{
	
}

bool FxSound::valid()const
{
	return sound != NULL;
}