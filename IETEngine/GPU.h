#ifndef GPU_h__
#define GPU_h__


template<class real>
struct GPUElement
{
	/// index of the 4 connected vertices
	//Vec<4,int> tetra;
	int ia[BSIZE];
	int ib[BSIZE];
	int ic[BSIZE];
	int id[BSIZE];
	/// material stiffness matrix
	//Mat<6,6,Real> K;
	real gamma_bx2[BSIZE], mu2_bx2[BSIZE];
	/// initial position of the vertices in the local (rotated) coordinate system
	//Vec3f initpos[4];
	real bx[BSIZE],cx[BSIZE];
	real cy[BSIZE],dx[BSIZE],dy[BSIZE],dz[BSIZE];
	/// strain-displacement matrix
	//Mat<12,6,Real> J;
	real Jbx_bx[BSIZE],Jby_bx[BSIZE],Jbz_bx[BSIZE];
};

template<class real>
struct GPUElementRotation
{
#ifdef USE_ROT6
	real rx[3][BSIZE];
	real ry[3][BSIZE];
#else
	real r[9][BSIZE];
#endif
};

template<class real>
struct GPUElementForce
{
	sofa::defaulttype::Vec<4,real> fA,fB,fC,fD;
};


template<class real>
struct GPUPlane
{
	real normal_x, normal_y, normal_z;
	real d;
	real stiffness;
	real damping;
};

template<class real>
struct GPUSphere
{
	real center_x, center_y, center_z;
	real velocity_x, velocity_y, velocity_z;
	real radius;
	real stiffness;
	real damping;
};
#endif // GPU_h__