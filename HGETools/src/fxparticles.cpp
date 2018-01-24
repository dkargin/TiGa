#include <fxobjects.h>

namespace Fx
{
///////////////////////////////////////////////////////
// Particle system
///////////////////////////////////////////////////////
FxParticles::FxParticles(FxManager * manager)
	:Base(manager), particles(NULL)
{}

FxParticles::FxParticles(const FxParticles &p)
	:particles(p.particles), Base(p)
{}

void FxParticles::init( hgeParticleSystemInfo & info )
{
	// TODO:
}
bool FxParticles::valid() const
{
	return true;
}

void FxParticles::update(float dt)
{
	if(!valid())
		return;
	Pose p=getGlobalPose();
	particles.MoveTo(p.position[0],p.position[1]);
	particles.Update(dt);
}

void FxParticles::render(const Pose &base)
{	
	particles.Render();
}

void FxParticles::start(AnimationMode mode)
{
	FxEffect::start(mode);

	Pose p = getGlobalPose();
	particles.FireAt(p.position[0],p.position[1]);	
}

void FxParticles::stop( bool immediate )
{
}

Time_t FxParticles::duration() const
{
	// TODO:
	return 1;
}

}
