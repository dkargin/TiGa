#pragma once
#include "../../sim/controllers/voController.h"

typedef VelocityObstacle VO2;
void rayCast(const std::vector<VO2> &vo,float angle,int steps,Rays &rays);
void drawRays(const Pose::vec &pos,const Rays &rays);
void drawVOs(const Pose::vec &pos,const std::vector<VO2> &vo);

struct VO2Controller: public Mover::Driver
{
public:		
	VO2Controller(Mover *parent): Mover::Driver(parent){}	
	void update(float dt);
	void render(HGE * hge);
	// 2� ������� Velocity Obstacle. ������� ������ ������ � ����������� 3�-2�	
protected:	
	Rays rays;	// {[angle,distance],...}
	std::vector<VO2> velocitySpace;
	void updateObstacles(int safetyLevel = 0);
	Segment calcSegment(const VelocityObstacle &vo)const ;
};