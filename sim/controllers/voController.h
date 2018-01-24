#pragma once
#include "../../sim/commandAI.h"
#include "Mover.h"


typedef FlatCone VelocityObstacle;

void addSegment(Segments & segments, const Geom::_Sphere<vec2f> &circle, const FlatCone &vo);
void rotate90(vec2f & vec);
void rotate90n(vec2f & vec);
const vec2f &to2d(const vec3 &v);

struct VOController: public Mover::Driver
{
public:		
	Segments segments;	
	//vector<VelocityObstacle> obstacles;	
	//VOController(Unit *parent):Controller(parent){}
	VOController(Mover* m);
	void update(float dt);
	void render(HGE * hge);
protected:	
	//void addObstacle(GameObject *object);
	
};

typedef VelocityObstacle VO2;

void rayCast(const std::vector<VO2> &vo,float angle,int steps,Rays &rays);
void drawRays(const Pose::vec &pos,const Rays &rays);
void drawVOs(const Pose::vec &pos,const std::vector<VO2> &vo);
void drawVO(HGE * hge,const VelocityObstacle &vo, float range, int steps);
// ���������� ����������� ����� �� ����� VO
void rayCast(const std::vector<VO2> &vo,float startAngle,int steps,Rays &rays);

void getMaxRanges(Rays &rays,float maxRange,std::vector<float> &distance);
// �������� ����� ���������� ��������� ���������
void getMinRanges(Rays &rays,float minRange,std::vector<float> &distance);