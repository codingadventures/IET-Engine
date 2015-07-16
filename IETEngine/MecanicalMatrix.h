#ifndef MechanicalMatrix_h__
#define MechanicalMatrix_h__


struct MechanicalMatrix
{
	double mFactor, kFactor;
	MechanicalMatrix(): mFactor(0), kFactor(0) {}
};

#endif // TimeIntegration_h__
