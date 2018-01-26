#pragma once

#include <vector>

#include "gameObject.h"

namespace sim
{

class VisionManager
{
public:
	struct VisionDesc
	{
		int player;
		float distance;
		float fov;
		Pose pose;
	};

	typedef std::vector<VisionDesc> Container;
	enum {ReservedVision=256};
	Container vision;

	VisionManager()
	{
		vision.reserve(ReservedVision);
	}

	virtual ~VisionManager() {}

	void clear()
	{
		vision.resize(0);
	}

	virtual bool allowVision(GameObject * owner)
	{
		return true;
	}

	virtual bool addVision(float distance, float fov,const Pose& pose, GameObject * owner)
	{
		if(owner && allowVision(owner))
		{
			VisionDesc res={ owner->getPlayer(), distance, fov, pose };
			vision.push_back(res);
			return true;
		}
		return false;
	}
};

}
