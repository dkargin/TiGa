#include "stdafx.h"
//#include "fxObjects.h"
//
////////////////////////////////////////////////////////////////////////////
//// FxBeam
//// for beamed effects. 
////////////////////////////////////////////////////////////////////////////
//
//FxBeam::FxBeam(FxManager *_manager):Base(manager)
//{
//	parts[0]=NULL;
//	parts[1]=NULL;
//	parts[2]=NULL;
//}
//FxBeam::FxBeam(const FxBeam &effect)
//	:Base(effect)
//{
//	for(int i=0;i<3;i++)
//		if(effect.parts[i])
//			parts[i]=new FxAnimation2(*effect.parts[i]);
//		else
//			parts[i]=NULL;
//}
//
//FxBeam::~FxBeam()
//{
//	for(int i=0;i<3;i++)
//		if(parts[i])
//			delete parts[i];
//}
//void FxBeam::setRay(const Pose::pos &from,const Pose::pos &to)
//{
//	pose.position=from;
//	pose.setDirection(to-from);
//	beamLength=(to-from).length();
//}
//void FxBeam::setLength(float length)
//{
//	beamLength=length;
//}
//Pose FxBeam::getStartPose()
//{
//	return pose;
//}
//Pose FxBeam::getEndPose()
//{
//	return pose*Pose(vec2f(beamLength,0),0);
//}
//void FxBeam::setPart(int i,FxEffect *part)
//{
//	FxAnimation2 *p=dynamic_cast<FxAnimation2*>(part);
//	if(p)
//		this->parts[i]=p->copy();
//}
//void FxBeam::start()
//{
//	for(int i=0;i<3;i++)
//		if(parts[i])parts[i]->start();
//}
//void FxBeam::stop()
//{
//	for(int i=0;i<3;i++)
//		if(parts[i])parts[i]->stop();
//}
//void FxBeam::rewind()
//{
//	for(int i=0;i<3;i++)
//		if(parts[i])parts[i]->rewind();
//}
//bool FxBeam::valid()const
//{
//	return true;
//}
//void FxBeam::update(float dt)
//{
//	for(int i=0;i<3;i++)
//		if(parts[i])parts[i]->update(dt);
//}
//void FxBeam::render(const Pose &base)
//{
//	if(!visible)return;
//	Pose p=base*getPose();
//	
//	if(parts[0])
//	{
//		//parts[0]->setPose(p);
//		parts[0]->render(p);
//	}
//	if(parts[2])
//	{
//		//parts[2]->setPose(p*Pose(vec2f(beamLength,0),0));
//		parts[2]->render(p*Pose(beamLength,0,0));
//	}
//	if(parts[1])
//	{		
//		float segment=parts[1]->getWidth();		
//		for(float l=0;l<beamLength;l+=segment)
//		{			
//			//parts[1]->crop=true;
//			parts[1]->crop=(beamLength-l<segment);
//			parts[1]->cropWidth=(beamLength-l<segment)?(beamLength-l)/segment:1.0;
//			//parts[1]->setPose(p*Pose(l,0,0));
//			//Pose bp=p*Pose(l+segment/2,0,0);
//			Pose bp=p*Pose(l+parts[1]->cropWidth*segment/2,0,0);
//			parts[1]->render(bp);		
//		}
//	}
//}
