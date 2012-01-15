#pragma once

class ForceController: public Controller
{
public:
	Pose::vec fObjects,fPath,fHit;
	std::list<Pose::vec> hits;					// possible hit positions
	ForceController(Unit *parent):Controller(parent){}
	void update(float dt);
	void render();
};