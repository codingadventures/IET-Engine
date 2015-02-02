#ifndef CollidingPair_h__
#define CollidingPair_h__

namespace Physics
{
	template <typename T>
	struct CollidingPair
	{
		T*			 m_left_element;
		T*			 m_right_element;
		EndPoint*	 m_end_point;

		CollidingPair(T* left_element,T* right_element);
	};

	template <typename T>
	Physics::CollidingPair<T>::CollidingPair(T* left_element,T* right_element)
		: m_left_element(left_element),
		 m_right_element(right_element)
	{

	}

}

#endif // CollidingPair_h__
