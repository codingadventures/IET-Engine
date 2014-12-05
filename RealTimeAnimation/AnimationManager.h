#ifndef AnimationManager_h__
#define AnimationManager_h__

#include <map>
#include <string>
#include "AnimationClip.h"


class AnimationManager
{
public:
	static std::map<string,AnimationClip*> AnimationSet;

};
std::map<string,AnimationClip*> AnimationManager::AnimationSet;

#endif // AnimationManager_h__
