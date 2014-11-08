#ifndef IANIMATION_H__
#define IANIMATION_H__

#include "Skeleton.h"
#include "Bone.h"

class IAnimation
{
protected:
	IAnimation(Skeleton* skeleton, glm::mat4 model): skeleton(skeleton), model(model){

	}
public:
	//Pure virtual functions
	virtual void Animate(glm::mat4 model,Bone* bone, glm::vec3 target = glm::vec3(), int numParents = 3) = 0;
	virtual void Animate(glm::mat4 model,float animationTime, glm::mat4* animationSequence)= 0;

protected:
	Skeleton* skeleton;
	glm::mat4 model;
};
#endif // IANIMATION_H__