#ifndef KEYFRAME_H__
#define KEYFRAME_H__


#include <glm/glm.hpp>  

struct KeyFrame
{
public:
	// Translation, Rotation and Scale of the animation. For each key frame defined in the modeler (blender in this case)
	// are defined a translation, two quaternions which are slerped (spherical interpolated, between two different key frames) and a scale operation.
	glm::vec3* pos_keys	;	
	double*	pos_key_times;	
	int	num_pos_keys	;

	glm::quat*	rot_keys;		
	double*	rot_key_times;	
	int	num_rot_keys	;

	glm::vec3*	sca_keys;		
	double*	sca_key_times;	
	int	num_sca_keys	;
};


#endif // KeyFrame_h__
