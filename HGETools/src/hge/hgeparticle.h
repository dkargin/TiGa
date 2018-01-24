/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class header
*/

#pragma once

//#include "hge.h"
#include <spritedata.h>
#include "color.h"
#include "rect.h"
#include "math3/math.h"

#define MAX_PARTICLES	500
#define MAX_PSYSTEMS	100

namespace Fx
{

struct Particle
{
	math3::vec2f	vecLocation;
	math3::vec2f	vecVelocity;

	float		fGravity;
	float		fRadialAccel;
	float		fTangentialAccel;

	float		fSpin;
	float		fSpinDelta;

	float		fSize;
	float		fSizeDelta;

	Color	colColor;				// + alpha
	Color	colColorDelta;

	float		fAge;
	float		fTerminalAge;
};

struct ParticleSystemInfo
{
	SpriteData*	sprite;		// texture + blend mode
	int			nEmission; 		// particles per sec
	float		fLifetime;

	float		fParticleLifeMin;
	float		fParticleLifeMax;

	float		fDirection;
	float		fSpread;
	bool		bRelative;

	float		fSpeedMin;
	float		fSpeedMax;

	float		fGravityMin;
	float		fGravityMax;

	float		fRadialAccelMin;
	float		fRadialAccelMax;

	float		fTangentialAccelMin;
	float		fTangentialAccelMax;

	float		fSizeStart;
	float		fSizeEnd;
	float		fSizeVar;

	float		fSpinStart;
	float		fSpinEnd;
	float		fSpinVar;

	Color	colColorStart; // + alpha
	Color	colColorEnd;
	float		fColorVar;
	float		fAlphaVar;
};

class hgeParticleSystem
{
public:
	hgeParticleSystemInfo info;
	
	hgeParticleSystem(const char *filename, SpriteData *sprite);
	hgeParticleSystem(hgeParticleSystemInfo *psi);
	hgeParticleSystem(const hgeParticleSystem &ps);
	~hgeParticleSystem() { hge->Release(); }

	hgeParticleSystem&	operator= (const hgeParticleSystem &ps);


	void				Render();
	void				FireAt(float x, float y);
	void				Fire();
	void				Stop(bool bKillParticles=false);
	void				Update(float fDeltaTime);
	void				MoveTo(float x, float y, bool bMoveParticles=false);
	void				Transpose(float x, float y) { fTx=x; fTy=y; }
	void				SetScale(float scale) { fScale = scale; }
	void				TrackBoundingBox(bool bTrack) { bUpdateBoundingBox=bTrack; }

	int					GetParticlesAlive() const { return nParticlesAlive; }
	float				GetAge() const { return fAge; }
	void				GetPosition(float *x, float *y) const { *x=vecLocation[0]; *y=vecLocation[1]; }
	void				GetTransposition(float *x, float *y) const { *x=fTx; *y=fTy; }
	float				GetScale() { return fScale; }
	hgeRect*			GetBoundingBox(hgeRect *rect) const;

private:
	hgeParticleSystem();

//	static HGE			*hge;

	float				fAge;
	float				fEmissionResidue;

	math3::vec2f		vecPrevLocation;
	math3::vec2f		vecLocation;
	float				fTx, fTy;
	float				fScale;

	int					nParticlesAlive;
	hgeRect				rectBoundingBox;
	bool				bUpdateBoundingBox;

	hgeParticle			particles[MAX_PARTICLES];
};

class hgeParticleManager
{
public:
	hgeParticleManager();
	~hgeParticleManager();

	void				Update(float dt);
	void				Render();

	hgeParticleSystem*	SpawnPS(hgeParticleSystemInfo *psi, float x, float y);
	bool				IsPSAlive(hgeParticleSystem *ps) const;
	void				Transpose(float x, float y);
	void				GetTransposition(float *dx, float *dy) const {*dx=tX; *dy=tY;}
	void				KillPS(hgeParticleSystem *ps);
	void				KillAll();

private:
	hgeParticleManager(const hgeParticleManager &);
	hgeParticleManager&	operator= (const hgeParticleManager &);

	int					nPS;
	float				tX;
	float				tY;
	hgeParticleSystem*	psList[MAX_PSYSTEMS];
};

}
