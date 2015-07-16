#ifndef FEMMesh_h__
#define FEMMesh_h__

#include "common.h"
#include "GPU.h"
#include <iostream>
#include "SimulationParameters.h"
#include <mesh/write_mesh_obj.h>

struct FEMMesh
{
	std::string filename;
	TVecCoord positions;
	TVecTetra tetrahedra;
	TVecTriangle triangles;
	TCoord bbox[2];
	TVecDeriv velocity;
	TVecCoord positions0; // rest positions

	// Description of external forces
	// In a real application, this could be extended to a different force for each particle
	struct ExternalForce
	{
		int index;
		TDeriv value;
	};
	ExternalForce externalForce;

	// Description of constraints
	int nbFixedParticles;
	MyVector(int) fixedParticles;
	MyVector(unsigned int) fixedMask;
	

	// Internal data and methods for simulation
	TVecDeriv f; // force vector when using Euler explicit
	TVecDeriv a,b; // solution and right-hand term when calling CG solver
	TVecDeriv r,d,q; // temporary vectors used by CG solver
#ifdef PARALLEL_REDUCTION
	TVecReal dottmp; // temporary buffer for dot product reductions
#endif

	// PlaneForceField
	GPUPlane<TReal> plane;
	TVecReal planePenetration;

	// SphereForceField
	GPUSphere<TReal> sphere;
	TVecReal spherePenetration;

	// TetrahedronFEMForceField
	MyVector(GPUElement<TReal>) femElem;
	MyVector(GPUElementRotation<TReal>) femElemRotation;
#ifdef PARALLEL_GATHER
	// data for parallel gather operation
	MyVector(GPUElementForce<TReal>) femElemForce;
	int nbElemPerVertex;
	MyVector(int) femVElems;
#endif

#ifdef USE_VEC4
	MyVector(TCoord4) x4,dx4;
#endif

	bool isFixedParticle(int index) const;
	void addFixedParticle(int index);
	void removeFixedParticle(int index);

	void reorder();

	void init(SimulationParameters* params);
	void update(SimulationParameters* params);

	void reset();
	void setPushForce(SimulationParameters* params);

	bool save(const std::string& filename);
	bool load(const std::string& filename);

	TReal tetraYoungModulus(int index, SimulationParameters* params)
	{
		const TReal youngModulusTop = (TReal)params->youngModulusTop;
		const TReal youngModulusBottom = (TReal)params->youngModulusBottom;
		TTetra t = tetrahedra[index];
		TReal y = (positions0[t[0]][1]+positions0[t[1]][1]+positions0[t[2]][1]+positions0[t[3]][1])*0.25f;
		y = (y - bbox[0][1])/(bbox[1][1]-bbox[0][1]);
		TReal youngModulus = youngModulusBottom + (youngModulusTop-youngModulusBottom) * y;
		return youngModulus;
	}
	TReal tetraPoissonRatio(int index, SimulationParameters* params)
	{
		return (TReal)params->poissonRatio;
	}

	void saveObj(const std::string& filename, const std::string& mtlfilename);

	FEMMesh()
	{
		nbFixedParticles = 0;
		externalForce.index = -1;
		plane.normal_x = 0;
		plane.normal_y = 1;
		plane.normal_z = 0;
		plane.d = 0;
		plane.stiffness = 0;
		plane.damping = 0;
		sphere.center_x = 0;
		sphere.center_y = 0;
		sphere.center_z = 0;
		sphere.velocity_x = 0;
		sphere.velocity_y = 0;
		sphere.velocity_z = 0;
		sphere.radius = 0;
		sphere.stiffness = 0;
		sphere.damping = 0;
#ifdef PARALLEL_GATHER
		nbElemPerVertex = 0;
#endif
	}

	private:
		double d_simulationSize;
};


template<class T1, class T2>
bool SortPairFirstFn(const std::pair<T1,T2>& a, const std::pair<T1,T2>& b) { return a.first < b.first; }

void FEMMesh::reorder()
{
	// simple reordering of the vertices using the largest dimension of the mesh
	int sort_coord = 0;
	if (bbox[1][1]-bbox[0][1] > bbox[1][sort_coord]-bbox[0][sort_coord])
		sort_coord = 1;
	if (bbox[1][2]-bbox[0][2] > bbox[1][sort_coord]-bbox[0][sort_coord])
		sort_coord = 2;
	std::cout << "Reordering particles based on " << (char)('X'+sort_coord) << std::endl;
	std::vector< std::pair<TReal,int> > sortp;
	sortp.resize(positions.size());
	for (unsigned int i=0;i<positions.size();++i)
		sortp[i] = std::make_pair(positions[i][sort_coord], i);
	std::sort(sortp.begin(),sortp.end(),SortPairFirstFn<TReal,int>);
	std::vector<int> old2newpos;
	old2newpos.resize(positions.size());
	for (unsigned int i=0;i<positions.size();++i)
		old2newpos[sortp[i].second] = i;
	TVecCoord newpos;
	newpos.resize(positions.size());
	for (unsigned int i=0;i<positions.size();++i)
		newpos[i] = positions[sortp[i].second];
	positions.swap(newpos);
	for (unsigned int i=0;i<tetrahedra.size();++i)
		for (unsigned int j=0;j<tetrahedra[i].size();++j)
			tetrahedra[i][j] = old2newpos[tetrahedra[i][j]];
	for (unsigned int i=0;i<triangles.size();++i)
		for (unsigned int j=0;j<triangles[i].size();++j)
			triangles[i][j] = old2newpos[triangles[i][j]];

	std::cout << "Reordering tetrahedra based on connected particles" << std::endl;
	std::vector< std::pair<int,int> > sortt;
	sortt.resize(tetrahedra.size());
	for (unsigned int i=0;i<tetrahedra.size();++i)
		sortt[i] = std::make_pair(std::min(std::min(tetrahedra[i][0],tetrahedra[i][1]),std::min(tetrahedra[i][2],tetrahedra[i][3])), i);
	std::sort(sortt.begin(),sortt.end(),SortPairFirstFn<int,int>);
	TVecTetra newtetra;
	newtetra.resize(tetrahedra.size());
	for (unsigned int i=0;i<tetrahedra.size();++i)
		newtetra[i] = tetrahedra[sortt[i].second];
	tetrahedra.swap(newtetra);
	std::cout << "Mesh reordering done" << std::endl;
}

bool FEMMesh::isFixedParticle(int index) const
{
	if (nbFixedParticles == 0) return false;
	// we use a bitmask instead of a indices vector to more easily search for the particle
	if (fixedMask.empty()) return false;
	int mi = index / 32;
	int mb = index % 32;
	if (fixedMask[mi] & (1 << mb)) return true;
	return false;

	//for (unsigned int i=0;i<fixedParticles.size();++i)
	//    if (fixedParticles[i] == index) return true;
	//return false;
}

void FEMMesh::addFixedParticle(int index)
{
	// for merged kernels we use a bitmask instead of a indices vector
	if (fixedMask.empty())
		fixedMask.resize((positions.size()+31) / 32);
	int mi = index / 32;
	int mb = index % 32;
	if (fixedMask[mi] & (1 << mb)) return; // already fixed
	fixedMask[mi] |= (1 << mb);

	// for standard kernels we use an indices vector
	fixedParticles.push_back(index);

	++nbFixedParticles;
}

void FEMMesh::removeFixedParticle(int index)
{
	// for merged kernels we use a bitmask instead of a indices vector
	if (fixedMask.empty()) return;
	int mi = index / 32;
	int mb = index % 32;
	if (!(fixedMask[mi] & (1 << mb))) return; // not fixed
	fixedMask[mi] &= ~(1 << mb);

	// for standard kernels we use an indices vector
	unsigned int i;
	for (i=0;i<fixedParticles.size();++i)
		if (fixedParticles[i] == index) break;
	if (i < fixedParticles.size())
	{
		if (i < fixedParticles.size()-1) // move to last
			fixedParticles[i] = fixedParticles[fixedParticles.size()-1];
		// and remove last
		fixedParticles.resize(fixedParticles.size()-1);
	}

	--nbFixedParticles;
}

void FEMMesh::init(SimulationParameters* params)
{
	if (positions0.size() != positions.size())
	{
		positions0 = positions;
		velocity.resize(positions.size());
	}

	const TVecCoord& x0 = positions0;
	d_simulationSize = params->simulation_size;
	// Plane
	plane.normal_x = 0;
	plane.normal_y = 1;
	plane.normal_z = 0;
	plane.d = params->plane_position[1];
	plane.stiffness = (TReal)params->planeRepulsion;
	plane.damping = 0;
	std::cout << "Plane d = " << plane.d << " stiffness = " << plane.stiffness << " damping = " << plane.damping << std::endl;

	// Fixed

	fixedParticles.clear();
	fixedMask.clear();
	nbFixedParticles = 0;
	if (params->fixedHeight > 0)
	{
		TReal maxY = (TReal)(bbox[0][1] + (bbox[1][1]-bbox[0][1]) * params->fixedHeight);
		TReal maxZ = (TReal)(bbox[0][2] + (bbox[1][2]-bbox[0][2]) * 0.75f);
		std::cout << "Fixed box = " << bbox[0] << "    " << TCoord(bbox[1][0],maxY,maxZ) << std::endl;
		for (unsigned int i=0;i<positions.size();++i)
			if (x0[i][1] <= maxY && x0[i][2] <= maxZ)
			{
				//fixedParticles.push_back(i);
				addFixedParticle(i);
				positions[i] = x0[i];
				if (!velocity.empty())
					velocity[i].clear();
			}
	}

	// Push Force
	setPushForce(params);

	// FEM

	const int nbp = positions.size();
	const int nbe = tetrahedra.size();
	const int nbBe = (nbe + BSIZE-1)/BSIZE;
	femElem.resize(nbBe);
	femElemRotation.resize(nbBe);
#ifdef PARALLEL_GATHER
	femElemForce.resize(nbe);
#endif
	// FEM matrices
	for (int eindex = 0; eindex < nbe; ++eindex)
	{
		TTetra& tetra = tetrahedra[eindex];
		//std::cout << "Elem " << eindex << " : indices = " << tetra << std::endl;
		TCoord A = x0[tetra[0]];
		TCoord B = x0[tetra[1]]-A;
		TCoord C = x0[tetra[2]]-A;
		TCoord D = x0[tetra[3]]-A;
		TReal vol36 = 6*dot( cross(B, C), D );
		if (vol36<0)
		{
			std::cerr << "ERROR: Negative volume for tetra "<<eindex<<" <"<<A<<','<<B<<','<<C<<','<<D<<"> = "<<vol36/36<<std::endl;
			vol36 *= -1;
			TCoord tmp = C; C = D; D = tmp;
			int itmp = tetra[2]; tetra[2] = tetra[3]; tetra[3] = itmp;
		}
		sofa::defaulttype::Mat<3,3,TReal> Rt;
		Rt[0] = B;
		Rt[2] = cross( B, C );
		Rt[1] = cross( Rt[2], B );
		Rt[0].normalize();
		Rt[1].normalize();
		Rt[2].normalize();
		TCoord b = Rt * B;
		TCoord c = Rt * C;
		TCoord d = Rt * D;
		//std::cout << "Elem " << eindex << " : b = " << b << "  c = " << c << "  d = " << d << std::endl;
		TReal y = (A[1] + (B[1]+C[1]+D[1])*0.25f - bbox[0][1])/(bbox[1][1]-bbox[0][1]);
		TReal youngModulus = tetraYoungModulus(eindex, params);
		TReal poissonRatio = tetraPoissonRatio(eindex, params);
		TReal gamma = (youngModulus*poissonRatio) / ((1+poissonRatio)*(1-2*poissonRatio));
		TReal mu2 = youngModulus / (1+poissonRatio);
		// divide by 36 times vol of the element
		gamma /= vol36;
		mu2 /= vol36;

		TReal bx2 = b[0] * b[0];

		const int block  = eindex / BSIZE;
		const int thread = eindex % BSIZE;
		GPUElement<TReal>& e = femElem[block];
		e.ia[thread] = tetra[0];
		e.ib[thread] = tetra[1];
		e.ic[thread] = tetra[2];
		e.id[thread] = tetra[3];
		e.bx[thread] = b[0];
		e.cx[thread] = c[0]; e.cy[thread] = c[1];
		e.dx[thread] = d[0]; e.dy[thread] = d[1]; e.dz[thread] = d[2];
		e.gamma_bx2[thread] = gamma * bx2;
		e.mu2_bx2[thread] = mu2 * bx2;
		e.Jbx_bx[thread] = (c[1] * d[2]) / b[0];
		e.Jby_bx[thread] = (-c[0] * d[2]) / b[0];
		e.Jbz_bx[thread] = (c[0]*d[1] - c[1]*d[0]) / b[0];
	}
#ifdef PARALLEL_GATHER
	// elements <-> particles table
	// first find number of elements per particle
	std::vector<int> p_nbe;
	p_nbe.resize(nbp);
	for (int eindex = 0; eindex < nbe; ++eindex)
		for (int j = 0; j < 4; ++j)
			++p_nbe[tetrahedra[eindex][j]];
	// then compute max value
	nbElemPerVertex = 0;
	for (int i=0;i<nbp;++i)
		if (p_nbe[i] > nbElemPerVertex) nbElemPerVertex = p_nbe[i];
#if GATHER_PT > 1
	// we will create group of GATHER_PT elements
	int nbElemPerThread = (nbElemPerVertex+GATHER_PT-1)/GATHER_PT;
	const int nbBpt = (nbp*GATHER_PT + GATHER_BSIZE-1)/GATHER_BSIZE;
	// finally fill velems array
	femVElems.resize(nbBpt*nbElemPerThread*GATHER_BSIZE);
#else
	const int nbBp = (nbp + GATHER_BSIZE-1)/GATHER_BSIZE;
	// finally fill velems array
	femVElems.resize(nbBp*nbElemPerVertex*GATHER_BSIZE);
#endif
	p_nbe.clear();
	p_nbe.resize(nbp);
	for (int eindex = 0; eindex < nbe; ++eindex)
		for (int j = 0; j < 4; ++j)
		{
			int p = tetrahedra[eindex][j];
			int num = p_nbe[p]++;
#if GATHER_PT > 1
			const int block  = (p*GATHER_PT) / GATHER_BSIZE;
			const int thread = (p*GATHER_PT+(num%GATHER_PT)) % GATHER_BSIZE;
			num = num/GATHER_PT;
			femVElems[ block * (nbElemPerThread * GATHER_BSIZE) +
				num * GATHER_BSIZE + thread ] = 1 + eindex * 4 + j;
#else
			const int block  = p / GATHER_BSIZE;
			const int thread = p % GATHER_BSIZE;
			femVElems[ block * (nbElemPerVertex * BSIZE) +
				num * BSIZE + thread ] = 1 + eindex * 4 + j;
#endif
		}
#endif
		std::cout << "FEM init done: " << positions.size() << " particles, " << tetrahedra.size() << " elements";
#ifdef PARALLEL_GATHER
		std::cout << ", up to " << nbElemPerVertex << " elements on each particle";
#endif
		std::cout << "." << std::endl;
		update(params);
}


void FEMMesh::update(SimulationParameters* params)
{

	// Sphere
	sphere.center_x = params->sphere_position[0];
	sphere.center_y = params->sphere_position[1];
	sphere.center_z = params->sphere_position[2];
	sphere.velocity_x = params->sphere_velocity[0];
	sphere.velocity_y = params->sphere_velocity[1];
	sphere.velocity_z = params->sphere_velocity[2];
	sphere.radius = params->sphere_radius;
	sphere.stiffness = (TReal)params->sphereRepulsion;
	sphere.damping = 0;
	//std::cout << "sphere r = " << sphere.radius << " stiffness = " << sphere.stiffness << " damping = " << sphere.damping << std::endl;
}

void FEMMesh::setPushForce(SimulationParameters* params)
{
	// find the most forward point in front of the mesh
	{
		int best = -1;
		TReal bestz = bbox[0][2];
		TReal minx = bbox[0][0] * 0.6f + bbox[1][0] * 0.4f;
		TReal maxx = bbox[0][0] * 0.4f + bbox[1][0] * 0.6f;
		for (unsigned int i=0;i<positions0.size();++i)
		{
			TCoord x = positions0[i];
			if (x[2] > bestz && x[0] >= minx && x[0] <= maxx)
			{
				best = i;
				bestz = x[2];
			}
		}
		externalForce.index = best;
		externalForce.value = params->pushForce * d_simulationSize;
	}
}

void FEMMesh::reset()
{
	positions = positions0;
	velocity.clear();
	velocity.resize(positions.size());
}

bool FEMMesh::save(const std::string& filename)
{
	std::ofstream out(filename.c_str());
	if (!out)
	{
		std::cerr << "Cannot write to file " << filename << std::endl;
		return false;
	}
	out << positions.size() << " " << 6 << std::endl;
	for (unsigned int i=0;i<positions.size();++i)
	{
		out << positions[i] << " " << velocity[i] << "\n";
	}
	out.flush();
	out.close();
	return true;
}

bool FEMMesh::load(const std::string& filename)
{
	std::ifstream in(filename.c_str());
	if (!in)
	{
		std::cerr << "Cannot open file " << filename << std::endl;
		return false;
	}
	int nbp = 0, nbc = 0;
	in >> nbp >> nbc;
	if (nbp != positions.size())
	{
		std::cerr << "ERROR: file " << filename << " contains " << nbp << " vertices while the mesh contains " << positions.size() << std::endl;
		return false;
	}
	if (nbc != 6)
	{
		std::cerr << "ERROR: file " << filename << " contains " << nbc << " values instead of 6" << std::endl;
		return false;
	}
	for (unsigned int i=0;i<positions.size();++i)
	{
		in >> positions[i] >> velocity[i];
	}
	in.close();
	return true;
}


void FEMMesh::saveObj(const std::string& filename, const std::string& mtlfilename)
{
	std::vector<int> smooth;
	smooth.push_back(-1);
	std::vector<std::string> groups;
	groups.push_back("Tetras");
	std::vector<std::string> mats;
	std::string mtlfile;
	std::string matname[4];
	if (!mtlfilename.empty())
	{
		matname[0] = "fD";
		matname[1] = "fA";
		matname[2] = "fB";
		matname[3] = "fC";
		TColor colors[4];
		colors[0] = TColor(0,0,1,1);
		colors[1] = TColor(0,0.5f,1,1);
		colors[2] = TColor(0,1,1,1);
		colors[3] = TColor(0.5f,1,1,1);
		std::size_t p = mtlfilename.rfind('/');
		mtlfile = std::string(mtlfilename, p == std::string::npos ? 0 : p+1);
		std::ofstream out(mtlfilename.c_str());
		if (out)
		{
			for (int m=0;m<4;++m)
			{
				out << "newmtl " << matname[m] << std::endl;
				TColor color = colors[m];
				out << "illum 2" << std::endl;
				out << "Ka " << color[0] << " " << color[1] << " " << color[2] << std::endl;
				out << "Kd " << color[0] << " " << color[1] << " " << color[2] << std::endl;
				out << "Ks " << 1 << " " << 1 << " " << 1 << std::endl;
				out << "Ns " << 20 << std::endl;
			}
			out.close();
		}
	}
	TVecCoord points; points.reserve(tetrahedra.size()*4);
	TVecTriangle triangles; triangles.reserve(tetrahedra.size()*4);
	mats.reserve(tetrahedra.size()*4);
	for (unsigned int i=0;i<tetrahedra.size();++i)
	{
		TTetra t = tetrahedra[i];

		TCoord a = positions[t[0]];
		TCoord b = positions[t[1]];
		TCoord c = positions[t[2]];
		TCoord d = positions[t[3]];
		TCoord center = (a+b+c+d)*(TReal)0.125;
		a = (a+center)*(TReal)0.666667;
		b = (b+center)*(TReal)0.666667;
		c = (c+center)*(TReal)0.666667;
		d = (d+center)*(TReal)0.666667;
		unsigned int pi = points.size();
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
		points.push_back(d);

		//glColor4f(0,0,1,1);
		//glVertex3fv(a.ptr()); glVertex3fv(b.ptr()); glVertex3fv(c.ptr());
		mats.push_back(matname[0]);
		triangles.push_back(TTriangle(pi+0, pi+1, pi+2));

		//glColor4f(0,0.5f,1,1);
		//glVertex3fv(b.ptr()); glVertex3fv(c.ptr()); glVertex3fv(d.ptr());
		mats.push_back(matname[1]);
		triangles.push_back(TTriangle(pi+1, pi+2, pi+3));

		//glColor4f(0,1,1,1);
		//glVertex3fv(c.ptr()); glVertex3fv(d.ptr()); glVertex3fv(a.ptr());
		mats.push_back(matname[2]);
		triangles.push_back(TTriangle(pi+2, pi+3, pi+0));

		//glColor4f(0.5f,1,1,1);
		//glVertex3fv(d.ptr()); glVertex3fv(a.ptr()); glVertex3fv(b.ptr());
		mats.push_back(matname[3]);
		triangles.push_back(TTriangle(pi+3, pi+0, pi+1));
	}
	write_mesh_obj(filename.c_str(), mtlfile.c_str(), points, triangles, (const TVecTexCoord*)NULL, &groups, &mats, &smooth);
}

#endif // FEMMesh_h__
