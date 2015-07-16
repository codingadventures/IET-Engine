#ifndef SIMULATION_H
#define SIMULATION_H

#include "common.h"

#include "FEMMesh.h"
#include "SimulationParameters.h"  
#include "SurfaceMesh.h"
#include "MecanicalMatrix.h"
#include "kernels.h"
#include <mesh/read_mesh_netgen.h> 
#include "cpu/CPUFixedConstraint.h"
#include "cpu/CPUMechanicalObject.h"
#include "cpu/CPUMergedKernels.h"
#include "cpu/CPUPlaneForceField.h"
#include "cpu/CPUSphereForceField.h"
#include "cpu/CPUTetrahedronFEMForceField.h"
#include "cpu/CPUUniformMass.h"
#include "cpu/CPUVisualModel.h"

class Simulation
{
public:
	bool simulation_preload();
	bool simulation_load_fem_mesh(const char* filename);
	void simulation_reorder_fem_mesh();
	bool simulation_load_render_mesh(const char* filename);
	bool simulation_init();

	void simulation_animate();
	void timeIntegrator_EulerImplicit(const SimulationParameters* params, FEMMesh* mesh);
	void timeIntegrator_EulerExplicit(const SimulationParameters* params, FEMMesh* mesh);
	void computeForce(const SimulationParameters* params, FEMMesh* mesh, TVecDeriv& result);
	void applyConstraints(const SimulationParameters* /*params*/, FEMMesh* mesh, TVecDeriv& result);
	void accFromF(const SimulationParameters* params, FEMMesh* mesh, const TVecDeriv& f);
	void addKv(const SimulationParameters* params, FEMMesh* mesh, double kFactor);
	void mulMatrixVector(const SimulationParameters* params, FEMMesh* mesh, MechanicalMatrix matrix, TVecDeriv& result, const TVecDeriv& input);
	void linearSolver_ConjugateGradient(const SimulationParameters* params, FEMMesh* mesh, MechanicalMatrix matrix);
	template<class TVec> void showDebug(const TVec& v, const char* name);
	void simulation_mapping();
	void simulation_reset();
	void simulation_save();
	void simulation_load();

	FEMMesh* fem_mesh;
	SimulationParameters simulation_params;
	//std::vector<SurfaceMesh*> render_meshes;
	std::vector<Mesh>* d_meshes;
	int d_verbose;
	Simulation(int verbose = 0);

	void SetMeshes(vector<Mesh>* meshes);
};


Simulation::Simulation(int verbose) :d_verbose(verbose)
{

}
void Simulation::SetMeshes(vector<Mesh>* meshes)
{
	d_meshes = meshes;
}


bool Simulation::simulation_preload()
{
	if (!kernels_init()) return false;
	return true;
}

bool Simulation::simulation_load_fem_mesh(const char* filename)
{
	FEMMesh* mesh = new FEMMesh;
	if (!read_mesh_netgen(filename, mesh->positions, mesh->tetrahedra, mesh->triangles))
	{
		delete mesh;
		return false;
	}
	mesh->filename = filename;

	if (mesh && mesh->positions.size() > 0)
	{
		mesh->bbox[0] = mesh->positions[0];
		mesh->bbox[1] = mesh->positions[0];
		for (unsigned int i=1;i<mesh->positions.size();++i)
		{
			TCoord p = mesh->positions[i];
			for (unsigned int c=0;c<p.size();++c)
				if (p[c] < mesh->bbox[0][c]) mesh->bbox[0][c] = p[c];
				else if (p[c] > mesh->bbox[1][c]) mesh->bbox[1][c] = p[c];
		}
	}

	fem_mesh = mesh;
	return true;
}

void Simulation::simulation_reorder_fem_mesh()
{
	if (fem_mesh)
		fem_mesh->reorder();
}

bool Simulation::simulation_init()
{
	FEMMesh* mesh = fem_mesh;

	simulation_params.Init(mesh->bbox);

	if (mesh)
	{
		mesh->init(&simulation_params);

		for (unsigned int i = 0; i < d_meshes->size(); ++i)
			(*d_meshes)[i].init(mesh);
	}

	return true;
}


void Simulation::simulation_reset()
{
	FEMMesh* mesh = fem_mesh;
	simulation_params.Reset();
	mesh->reset();
	mesh->setPushForce(&simulation_params);
	mesh->update(&simulation_params);
}

void Simulation::simulation_save()
{
	FEMMesh* mesh = fem_mesh;
	if (simulation_params.simulation_time)
		mesh->save(mesh->filename + ".state");
	simulation_mapping();
	std::string suffix = (simulation_params.simulation_time ? "-deformed" : "-initial");
	/*for (unsigned int i = 0; i < meshes.size(); ++i)
	{
	std::string filename(meshes[i]->filename, 0, render_meshes[i]->filename.size()-4);
	meshes[i]->saveObj(filename + suffix + ".obj", filename + suffix + ".mtl");
	}*/
	{
		std::string filename(mesh->filename, 0, mesh->filename.size()-5);
		mesh->saveObj(filename + suffix + ".obj", filename + suffix + ".mtl");
	}
}

void Simulation::simulation_load()
{
	FEMMesh* mesh = fem_mesh;
	mesh->load(mesh->filename + ".state");
	simulation_params.simulation_mapping_needed = true;
	simulation_params.simulation_time = 1;
}


void Simulation::simulation_mapping()
{
	if (!simulation_params.simulation_mapping_needed) return;

	simulation_params.simulation_mapping_needed = false;
	FEMMesh* mesh = fem_mesh;
	if (!mesh) return;
	for (unsigned int i = 0; i < d_meshes->size(); ++i)
	{
		(*d_meshes)[i].updatePositions(mesh);
		//meshes[i]->updateNormals();

	}
}
//
void Simulation::simulation_animate()
{
	FEMMesh* mesh = fem_mesh;
	if (!mesh) return;
	switch (simulation_params.odeSolver)
	{
	case ODE_EulerExplicit:
		timeIntegrator_EulerExplicit(&simulation_params, mesh);
		break;
	case ODE_EulerImplicit:
		timeIntegrator_EulerImplicit(&simulation_params, mesh);
		break;
	}

	// non-simulated objects
	simulation_params.sphere_position += simulation_params.sphere_velocity * simulation_params.timeStep;
	mesh->update(&simulation_params);

	simulation_params.simulation_time += simulation_params.timeStep;
	simulation_params.simulation_mapping_needed = true;
}


void Simulation::timeIntegrator_EulerImplicit(const SimulationParameters* params, FEMMesh* mesh)
{
	const double h  = params->timeStep;
	const double rM = params->rayleighMass;
	const double rK = params->rayleighStiffness;

	// Compute right-hand term b
	TVecDeriv& b = mesh->b;
	computeForce(params, mesh, b);
	// no need to apply constraints as it will be done in addKv()
	addKv(params, mesh, h);

	// Compute matrix
	MechanicalMatrix systemMatrix;
	systemMatrix.mFactor = 1 - h*rM;
	systemMatrix.kFactor =   - h*rK - h*h;

	// Solve system for a
	linearSolver_ConjugateGradient(params, mesh, systemMatrix);

	// Apply solution:  v = v + h a        x = x + h v
	TVecCoord& x = mesh->positions;
	TVecDeriv& v = mesh->velocity;
	const TVecDeriv& a = mesh->a;
#ifdef MERGE_CG_KERNELS
	DEVICE_METHOD(MechanicalObject3f_vIntegrate)( x.size(), a.deviceRead(), v.deviceWrite(), x.deviceWrite(), (TReal)h );
#else
	DEVICE_METHOD(MechanicalObject3f_vPEqBF)( x.size(), v.deviceWrite(), a.deviceRead(), (TReal)h );
	DEVICE_METHOD(MechanicalObject3f_vPEqBF)( x.size(), x.deviceWrite(), v.deviceRead(), (TReal)h );
#endif

}

void Simulation::timeIntegrator_EulerExplicit(const SimulationParameters* params, FEMMesh* mesh)
{
	const double h  = params->timeStep;
	const double rM = params->rayleighMass;
	TVecCoord& x = mesh->positions;
	TVecDeriv& v = mesh->velocity;
	TVecDeriv& f = mesh->f;
	TVecDeriv& a = mesh->a;

	// Compute force
	computeForce(params, mesh, f);

	// Apply constraints
	applyConstraints(params, mesh, f);

	// Compute acceleration
	accFromF(params, mesh, f);

	// Apply damping
	if (rM != 0.0)
		DEVICE_METHOD(MechanicalObject3f_vEqBF)( v.size(), v.deviceWrite(), v.deviceRead(), 1-rM);

	// Apply solution:  v = v + h a        x = x + h v
	DEVICE_METHOD(MechanicalObject3f_vPEqBF)( x.size(), v.deviceWrite(), a.deviceRead(), (TReal)h );
	DEVICE_METHOD(MechanicalObject3f_vPEqBF)( x.size(), x.deviceWrite(), v.deviceRead(), (TReal)h );
}


// Compute b = f
void Simulation::computeForce(const SimulationParameters* params, FEMMesh* mesh, TVecDeriv& result)
{
	const unsigned int size = mesh->positions.size();
	const double mass = params->massDensity;
	const TDeriv mg = params->gravity * mass;
	const TVecCoord& x = mesh->positions;
	const TVecDeriv& v = mesh->velocity;
	TVecDeriv& f = result;
	result.recreate(size);

	// it is no longer necessary to clear the result vector as the addForce
	// kernel from TetrahedronFEMForceField will do an assignement instead of
	// an addition
	if (params->youngModulusTop == 0 && params->youngModulusBottom == 0)
		DEVICE_METHOD(MechanicalObject3f_vClear)( size, result.deviceWrite() );

	// Internal Forces
	// result = f(x,v)
	if (params->youngModulusTop != 0 || params->youngModulusBottom != 0)
	{
#ifdef USE_VEC4
		mesh->x4.fastResize(size);
		DEVICE_METHOD(TetrahedronFEMForceField3f_prepareX)(size, mesh->x4.deviceWrite(), x.deviceRead());
#endif
		DEVICE_METHOD(TetrahedronFEMForceField3f_addForce)( mesh->tetrahedra.size(), size, false
			, mesh->femElem.deviceRead(), mesh->femElemRotation.deviceWrite()
			, result.deviceWrite(), x.deviceRead()
#ifdef PARALLEL_GATHER
			, mesh->nbElemPerVertex, GATHER_PT, GATHER_BSIZE
			, mesh->femElemForce.deviceWrite(), mesh->femVElems.deviceRead()
#endif
			);
	}

	// External forces
	if (mesh->externalForce.index >= 0)
	{
		//result[mesh->externalForce.index] += mesh->externalForce.value;
		DEVICE_METHOD(MechanicalObject3f_vPEq1)( size, result.deviceWrite(), mesh->externalForce.index, mesh->externalForce.value.ptr() );
	}

	if (mesh->plane.stiffness != 0)
	{
		mesh->planePenetration.recreate(size);
		DEVICE_METHOD(PlaneForceField3f_addForce)( size, &mesh->plane, mesh->planePenetration.deviceWrite(), result.deviceWrite(), x.deviceRead(), v.deviceRead() );
	}

	if (mesh->sphere.stiffness != 0)
	{
		mesh->spherePenetration.recreate(size);
		DEVICE_METHOD(SphereForceField3f_addForce)( size, &mesh->sphere, mesh->spherePenetration.deviceWrite(), result.deviceWrite(), x.deviceRead(), v.deviceRead() );
	}

	// Gravity
	DEVICE_METHOD(UniformMass3f_addForce)( size, mg.ptr(), result.deviceWrite() );
}

void Simulation::applyConstraints(const SimulationParameters* /*params*/, FEMMesh* mesh, TVecDeriv& result)
{
	if (mesh->nbFixedParticles > 0)
	{
		DEVICE_METHOD(FixedConstraint3f_projectResponseIndexed)( mesh->fixedParticles.size(), mesh->fixedParticles.deviceRead(), result.deviceWrite() );
	}
}

// Compute a = M^-1 f
void Simulation::accFromF(const SimulationParameters* params, FEMMesh* mesh, const TVecDeriv& f)
{
	const unsigned int size = mesh->positions.size();
	const double mass = params->massDensity;
	TVecDeriv& a = mesh->a;
	a.recreate(size);
	DEVICE_METHOD(UniformMass3f_addMDx)( size, mass, a.deviceWrite(), f.deviceRead() );
}

// Compute b += kFactor * K * v
void Simulation::addKv(const SimulationParameters* params, FEMMesh* mesh, double kFactor)
{
	const unsigned int size = mesh->positions.size();
	const double mass = params->massDensity;
	const TDeriv mg = params->gravity * mass;
	const TVecCoord& x = mesh->positions;
	const TVecDeriv& v = mesh->velocity;
	TVecDeriv& b = mesh->b;

	// b += kFactor * K * v
	if (params->youngModulusTop != 0 || params->youngModulusBottom != 0)
	{
#ifdef USE_VEC4
		mesh->dx4.fastResize(size);
		DEVICE_METHOD(TetrahedronFEMForceField3f_prepareDx)(size, mesh->dx4.deviceWrite(), v.deviceRead());
#endif
		DEVICE_METHOD(TetrahedronFEMForceField3f_addDForce)( mesh->tetrahedra.size(), size, true, kFactor
			, mesh->femElem.deviceRead(), mesh->femElemRotation.deviceRead()
			, b.deviceWrite(), v.deviceRead()
#ifdef PARALLEL_GATHER
			, mesh->nbElemPerVertex, GATHER_PT, GATHER_BSIZE
			, mesh->femElemForce.deviceWrite(), mesh->femVElems.deviceRead()
#endif
			);
	}

	if (mesh->plane.stiffness != 0)
	{
		GPUPlane<TReal> plane2 = mesh->plane;
		plane2.stiffness *= (TReal)kFactor;
		DEVICE_METHOD(PlaneForceField3f_addDForce)( size, &plane2, mesh->planePenetration.deviceRead(), b.deviceWrite(), v.deviceRead() );
	}

	if (mesh->sphere.stiffness != 0)
	{
		GPUSphere<TReal> sphere2 = mesh->sphere;
		sphere2.stiffness *= (TReal)kFactor;
		DEVICE_METHOD(SphereForceField3f_addDForce)( size, &sphere2, mesh->spherePenetration.deviceRead(), b.deviceWrite(), v.deviceRead() );
	}

	if (mesh->nbFixedParticles > 0)
	{
		DEVICE_METHOD(FixedConstraint3f_projectResponseIndexed)( mesh->fixedParticles.size(), mesh->fixedParticles.deviceRead(), b.deviceWrite() );
	}

	//std::cout << "b = " << b << std::endl;
	//if (d_verbose >= 2) showDebug(b, "b");

}

void Simulation::mulMatrixVector(const SimulationParameters* params, FEMMesh* mesh, MechanicalMatrix matrix, TVecDeriv& result, const TVecDeriv& input)
{
	const unsigned int size = mesh->positions.size();
	const double mass = params->massDensity;

	//    std::cout << "matrix = " << matrix.mFactor << " * " << mass << " = " << matrix.mFactor * mass << std::endl;

	// it is no longer necessary to clear the result vector as the addDForce
	// kernel from TetrahedronFEMForceField will do an assignement instead of
	// an addition
	if (params->youngModulusTop == 0 && params->youngModulusBottom == 0)
		DEVICE_METHOD(MechanicalObject3f_vClear)( size, result.deviceWrite() );

	if (params->youngModulusTop != 0 || params->youngModulusBottom != 0)
	{
#ifdef USE_VEC4
		mesh->dx4.fastResize(size);
		DEVICE_METHOD(TetrahedronFEMForceField3f_prepareDx)(size, mesh->dx4.deviceWrite(), input.deviceRead());
#endif
		DEVICE_METHOD(TetrahedronFEMForceField3f_addDForce)( mesh->tetrahedra.size(), size, false, matrix.kFactor
			, mesh->femElem.deviceRead(), mesh->femElemRotation.deviceRead()
			, result.deviceWrite(), input.deviceRead()
#ifdef PARALLEL_GATHER
			, mesh->nbElemPerVertex, GATHER_PT, GATHER_BSIZE
			, mesh->femElemForce.deviceWrite(), mesh->femVElems.deviceRead()
#endif
			);
	}

	DEVICE_METHOD(UniformMass3f_addMDx)( size, matrix.mFactor * mass, result.deviceWrite(), input.deviceRead() );

	if (mesh->plane.stiffness != 0)
	{
		GPUPlane<TReal> plane2 = mesh->plane;
		plane2.stiffness *= matrix.kFactor;
		DEVICE_METHOD(PlaneForceField3f_addDForce)( size, &plane2, mesh->planePenetration.deviceRead(), result.deviceWrite(), input.deviceRead() );
	}

	if (mesh->sphere.stiffness != 0)
	{
		GPUSphere<TReal> sphere2 = mesh->sphere;
		sphere2.stiffness *= matrix.kFactor;
		DEVICE_METHOD(SphereForceField3f_addDForce)( size, &sphere2, mesh->spherePenetration.deviceRead(), result.deviceWrite(), input.deviceRead() );
	}

	if (mesh->nbFixedParticles > 0)
	{
		DEVICE_METHOD(FixedConstraint3f_projectResponseIndexed)( mesh->fixedParticles.size(), mesh->fixedParticles.deviceRead(), result.deviceWrite() );
	}
}

int simulation_cg_iter = 0;

void Simulation::linearSolver_ConjugateGradient(const SimulationParameters* params, FEMMesh* mesh, MechanicalMatrix matrix)
{
	const unsigned int size = mesh->positions.size();
	const double mass = params->massDensity;
	const int maxIter = params->maxIter;
	const double tolerance = params->tolerance;
	const TVecDeriv& b = mesh->b;
	TVecDeriv& a = mesh->a;
	TVecDeriv& q = mesh->q;
	TVecDeriv& d = mesh->d;
	TVecDeriv& r = mesh->r;
	a.recreate(size);
	q.recreate(size);
	d.recreate(size);
	r.recreate(size);

	// for parallel reductions (vDot)
#ifdef PARALLEL_REDUCTION
	TVecReal& tmp = mesh->dottmp;
	int tmpsize = std::max(
		DEVICE_METHOD(MechanicalObject3f_vDotTmpSize)( size ),
#if defined(MERGE_REDUCTION_KERNELS)
		DEVICE_METHOD(MergedKernels3f_cgDot3TmpSize)( size )
#elif defined(MERGE_CG_KERNELS)
		DEVICE_METHOD(MergedKernels3f_cgDeltaTmpSize)( size )
#else
		0
#endif
		);
	tmp.recreate(tmpsize);
	DEVICE_PTR(TReal) dottmp = tmp.deviceWrite();
	TReal* cputmp = (TReal*)(&(tmp.getCached(0)));
#endif
	float dotresult = 0;
#if defined(MERGE_REDUCTION_KERNELS)
	float dot3result[3] = {0,0,0};
#endif

	int i = 0;

	// Here we assume a initial guess of 0
	// Therefore we can replace "r = b - Aa" in the initial algorithm by "r = b"
	// As a consequence, we also do not copy b to r and d before the loop, but use
	// it directly in the first iteration

	// r = b - Aa;
	// d = r;
	// => d = r = b

	// delta0 = dot(r,r) = dot(b,b);
	DEVICE_METHOD(MechanicalObject3f_vDot)( size, &dotresult, b.deviceRead(), b.deviceRead()
#ifdef PARALLEL_REDUCTION
		, dottmp, cputmp
#endif
		);
	double delta_0 = dotresult;

	if (d_verbose >= 2) std::cout << "CG Init delta = " << delta_0 << std::endl;
	const double delta_threshold = delta_0 * (tolerance * tolerance);
	double delta_new = delta_0;
	if (delta_new <= delta_threshold) // no iteration, solution is 0
	{
		DEVICE_METHOD(MechanicalObject3f_vClear)( size, a.deviceWrite() );
		simulation_cg_iter = 0;
		return;
	}
	while (i < maxIter && delta_new > delta_threshold)
	{
		// q = Ad;
		mulMatrixVector(params, mesh, matrix, q, ((i==0)?b:d));
		if (d_verbose >= 2) showDebug(q, "q");
#if defined(MERGE_REDUCTION_KERNELS)
		if (i==0)
			DEVICE_METHOD(MergedKernels3f_cgDot3First)( size, dot3result
			, b.deviceRead(), q.deviceRead()
#ifdef PARALLEL_REDUCTION
			, dottmp, cputmp
#endif
			);
		else
			DEVICE_METHOD(MergedKernels3f_cgDot3)( size, dot3result
			, r.deviceRead(), q.deviceWrite(), d.deviceRead()
#ifdef PARALLEL_REDUCTION
			, dottmp, cputmp
#endif
			);

		double dot_dq = dot3result[0];
		double dot_rq = dot3result[1];
		double dot_qq = dot3result[2];
		double den = dot_dq;
		if (d_verbose >= 2) std::cout << "CG i="<<i<<" den = " << den << std::endl;
		double alpha = delta_new / den;
		if (d_verbose >= 2) std::cout << "CG i="<<i<<" alpha = " << alpha << std::endl;
		double delta_old = delta_new;
		// r_new = r - q * alpha
		// delta_new = dot(r_new,r_new) = dot(r - q * alpha,r - q * alpha) = dot(r,r) -2*alpha*dot(r,q) + alpha^2*(dot(q,q)
		delta_new = delta_old - 2*alpha*dot_rq + alpha*alpha*dot_qq;

		if (d_verbose >= 2) std::cout << "CG i="<<i<<" delta = " << delta_new << std::endl;

		double beta = delta_new / delta_old;
		// a = a + d * alpha;
		// r = r - q * alpha;
		// d = r + d * beta;
		if (i==0)
			DEVICE_METHOD(MergedKernels3f_cgOp3First)( size, alpha, beta
			, r.deviceWrite(), a.deviceWrite(), d.deviceWrite(), q.deviceRead(), b.deviceRead()
			);
		else
			DEVICE_METHOD(MergedKernels3f_cgOp3)( size, alpha, beta
			, r.deviceWrite(), a.deviceWrite(), d.deviceWrite(), q.deviceRead()
			);

		if (d_verbose >= 2) showDebug(a, "a");
		if (d_verbose >= 2) showDebug(r, "r");
		if (d_verbose >= 2) showDebug(d, "d");

#else
		// den = dot(d,q)
		DEVICE_METHOD(MechanicalObject3f_vDot)( size, &dotresult, ((i==0)?b.deviceRead():d.deviceRead()), q.deviceRead()
#ifdef PARALLEL_REDUCTION
			, dottmp, cputmp
#endif
			);
		double den = dotresult;
		if (verbose >= 2) std::cout << "CG i="<<i<<" den = " << den << std::endl;
		double alpha = delta_new / den;
		if (verbose >= 2) std::cout << "CG i="<<i<<" alpha = " << alpha << std::endl;
		double delta_old = delta_new;
		// a = a + d * alpha
		// r = r - q * alpha
		// delta_new = dot(r,r)
#if defined(MERGE_CG_KERNELS)
		DEVICE_METHOD(MergedKernels3f_cgDelta)( (i==0), size, &dotresult, (TReal)alpha
			, r.deviceWrite(), a.deviceWrite(), q.deviceRead(), ((i==0)?b.deviceRead():d.deviceRead())
#ifdef PARALLEL_REDUCTION
			, dottmp, cputmp
#endif
			);
		delta_new = dotresult;
#else
		if (i==0)
		{
			DEVICE_METHOD(MechanicalObject3f_vEqBF)( size, a.deviceWrite(), b.deviceRead(), alpha );
			DEVICE_METHOD(MechanicalObject3f_vOp)( size, r.deviceWrite(), b.deviceRead(), q.deviceRead(), -alpha );
		}
		else
		{
			DEVICE_METHOD(MechanicalObject3f_vPEqBF)( size, a.deviceWrite(), d.deviceRead(), alpha );
			DEVICE_METHOD(MechanicalObject3f_vPEqBF)( size, r.deviceWrite(), q.deviceRead(), -alpha );
		}
		DEVICE_METHOD(MechanicalObject3f_vDot)( size, &dotresult, r.deviceRead(), r.deviceRead()
#ifdef PARALLEL_REDUCTION
			, dottmp, cputmp
#endif
			);
		delta_new = dotresult;
#endif

		if (verbose >= 2) showDebug(a, "a");
		if (verbose >= 2) showDebug(r, "r");

		if (verbose >= 2) std::cout << "CG i="<<i<<" delta = " << delta_new << std::endl;
		double beta = delta_new / delta_old;
		// d = r + d * beta;
		if (i==0)
			DEVICE_METHOD(MechanicalObject3f_vOp)( size, d.deviceWrite(), r.deviceRead(), b.deviceRead(), (TReal)beta );
		else
			DEVICE_METHOD(MechanicalObject3f_vOp)( size, d.deviceWrite(), r.deviceRead(), d.deviceRead(), (TReal)beta );
		if (verbose >= 2) showDebug(d, "d");
#endif
		++i;
	}
	simulation_cg_iter = i;
	if (d_verbose >= 1) std::cout << "CG iterations = " << i << " residual error = " << sqrt(delta_new / delta_0) << std::endl;
}


template<class TVec>
void Simulation::showDebug(const TVec& v, const char* name)
{
	std::cout << name << " =";
	if (v.size() < 10) std::cout << ' ' << v;
	else
	{
		for (unsigned int i=0;i<5;++i) std::cout << ' ' << v[i];
		std::cout << " ...";
		for (unsigned int i=v.size()-5;i<v.size();++i) std::cout << ' ' << v[i];
	}
	std::cout << std::endl;
}
#endif
