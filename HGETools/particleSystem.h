/*
 * particleSystem.h
 *
 *  Created on: Jan 23, 2018
 *      Author: vrobot
 */

#pragma once

#include <fxobjects.h>
#include <hge_old/hgeparticle.h>

class ParticleSystemInfo;

namespace Fx
{
	class FxParticles: public Entity
	{
	public:
		struct Particle
		{
			math3::vec2f position;
			math3::vec2f velocity;
			float angle;			// model angle
			// Should it be here?
			FxParticles * master;
		};

		ParticleSystem particles;

		FX_TYPE(FxParticles, Entity::fxParticles);

		FxParticles(FxManager * manager);
		FxParticles(const FxParticles &p);

		void init(Fx::ParticleSystemInfo &info);

		virtual bool valid() const;
		virtual void update(float dt);
		virtual void render(const Pose &base);
		/// animation interface
		virtual void start(AnimationMode mode = AnimationNoChange);
		virtual void stop(bool immediate);
		virtual Time_t duration() const;
	};
}
