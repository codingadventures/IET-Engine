#ifndef KEYFRAMEANIMATOR_H__
#define KEYFRAMEANIMATOR_H__

#include "IAnimation.h"



class KeyFrameAnimator : IAnimation
{



	virtual void Animate(glm::mat4 model,Bone* bone, glm::vec3 target = glm::vec3() , int numParents = 3)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void Animate(glm::mat4 model, float animationTime, glm::mat4* animationSequence)
	{
		assert(animationSequence);
		this->animationSequence = animationSequence;

		animateKeyFrames(skeleton->rootBone,animationTime);
	}

	private:
		glm::mat4* animationSequence;

		void animateKeyFrames(Bone* bone, float animationTime){
			 
			/* the animation for a particular bone at this time */
			glm::mat4 local_anim;

			glm::mat4 bone_T;
			if (bone->num_pos_keys > 0) {
				int prev_key = 0;
				int next_key = 0;
				for (int i = 0; i < bone->num_pos_keys - 1; i++) {
					prev_key = i;
					next_key = i + 1;
					if (bone->pos_key_times[next_key] >= animationTime) {
						break;
					}
				}
				float total_t = bone->pos_key_times[next_key] - bone->pos_key_times[prev_key];
				float t = (animationTime - bone->pos_key_times[prev_key]) / total_t;
				glm::vec3 vi = bone->pos_keys[prev_key];
				glm::vec3 vf = bone->pos_keys[next_key];
				glm::vec3 lerped = vi * (1.0f - t) + vf * t;
				bone_T = glm::translate(glm::mat4(), lerped);
			}

			glm::mat4 bone_R;
			if (bone->num_rot_keys > 0) {
				// find next and previous keys
				int prev_key = 0;
				int next_key = 0;
				for (int i = 0; i < bone->num_rot_keys - 1; i++) {
					prev_key = i;
					next_key = i + 1;
					if (bone->rot_key_times[next_key] >= animationTime) {
						break;
					}
				}
				float total_t = bone->rot_key_times[next_key] - bone->rot_key_times[prev_key];
				float t = (animationTime - bone->rot_key_times[prev_key]) / total_t;
				glm::quat qi = bone->rot_keys[prev_key];
				glm::quat qf = bone->rot_keys[next_key];
				glm::quat slerped = glm::slerp (qi, qf, t);
				bone_R = glm::toMat4(slerped);
			}

			local_anim = bone_T * bone_R;

			// if bone has a weighted bone...
			int bone_i = bone->boneIndex;
			if (bone_i > -1) {
				// ... then get offset matrices
				glm::mat4 bone_offset = bone->boneOffset;;

				bone->finalTransform = bone->parent->finalTransform * local_anim;
				animationSequence[bone_i] = bone->finalTransform  * bone_offset; //Change this to match OGL


			}
			for (int i = 0; i < bone->children.size(); i++) {
				animateKeyFrames (&bone->children[i],animationTime);
			}
		}
};
#endif // KEYFRAMEANIMATOR_H__