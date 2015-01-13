#ifndef KEYFRAME_H__
#define KEYFRAME_H__


#include <glm/glm.hpp> 
#include <glm/gtx/quaternion.hpp>

struct KeyFrame
{
	protected:
	// Translation, Rotation and Scale of the animation. For each key frame defined in the modeler (blender in this case)
	// are defined a translation, two quaternions which are slerped (spherical interpolated, between two different key frames) and a scale operation.
	double mTime;
	
public:
	double GetTime()
	{
		return mTime;
	}
};

struct RotationKeyFrame : KeyFrame
{
public: 
	 
	glm::quat	mRotation;		
	RotationKeyFrame(glm::quat rotation,double time) : mRotation(rotation)
	{
		mTime=time;
	}
};

struct TranslationKeyFrame : KeyFrame
{
public: 

	glm::vec3	mTranslation;		  
	TranslationKeyFrame(glm::vec3 translation,double time) : mTranslation(translation)
	{
		 mTime=time;
	}
};

struct ScaleKeyFrame : KeyFrame
{
public: 

	glm::vec3	mScale;		
	ScaleKeyFrame(glm::vec3 scale,double time) : mScale(scale)
	{
		mTime=time;
	}
};

#endif // KeyFrame_h__
