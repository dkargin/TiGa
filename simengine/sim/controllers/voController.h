#pragma once
#include "../commandAI.h"
#include "../mover.h"


namespace sim
{

typedef FlatCone VelocityObstacle;

void addSegment(math3::Segments & segments, const Sphere2 &circle, const FlatCone &vo);
void rotate90(vec2f & vec);
void rotate90n(vec2f & vec);

struct VOController: public Mover::Driver
{
public:		
	math3::Segments segments;
	//vector<VelocityObstacle> obstacles;	
	//VOController(Unit *parent):Controller(parent){}
	VOController(Mover* m);
	void update(float dt);
	void render(Fx::RenderContext* rc);
protected:	
	//void addObstacle(GameObject *object);
	
};

typedef VelocityObstacle VO2;

void rayCast(const std::vector<VO2> &vo,float angle,int steps,Rays &rays);
void drawRays(const Pose::vec &pos,const Rays &rays);
void drawVOs(const Pose::vec &pos,const std::vector<VO2> &vo);
void drawVO(Fx::RenderContext* rc,const VelocityObstacle &vo, float range, int steps);
// ���������� ����������� ����� �� ����� VO
void rayCast(const std::vector<VO2> &vo,float startAngle,int steps,Rays &rays);

void getMaxRanges(Rays &rays,float maxRange,std::vector<float> &distance);
// �������� ����� ���������� ��������� ���������
void getMinRanges(Rays &rays,float minRange,std::vector<float> &distance);

}
