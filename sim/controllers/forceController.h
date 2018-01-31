#pragma once

#include "../basetypes.h"

namespace sim
{

class ForceController: public Controller
{
public:
	Pose::vec fObjects,fPath,fHit;
	std::list<vec2f> hits;					// possible hit positions
	ForceController(Unit* parent);
	void update(float dt);
	void render();
};

};
