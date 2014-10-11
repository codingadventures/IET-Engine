#pragma once 

#include "Bone.h"
#include "Skeleton.h"
#include "BonePose.h"
#include "glm/glm.hpp"
#include <map>

class SkeletonPose {
	public:
	std::map<std::string, BonePose*> bonePoses;
	SkeletonPose();
	SkeletonPose(Skeleton* skeleton);
	~SkeletonPose();
	void calculateBonePose(Bone* bone, Bone* parentBone);
	void calculateDisplayMatrix(Bone* bone, Bone* parentBone, glm::mat4 mats);
};