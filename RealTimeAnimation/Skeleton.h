#pragma once

#include "Bone.h"
#include <vector>

class Skeleton 
{
public:
	Bone* rootBone;
	std::map<std::string, glm::uint> boneMapping;
	//std::map<std::string, Bone*> bones;
	Skeleton(){}
	Skeleton(Bone* rootBone): rootBone(rootBone)
	{
		 
	}

private:
	//vector<Bone*> bones;

};